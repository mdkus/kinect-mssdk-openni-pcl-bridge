//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#pragma once
#include "base.h"

#include "DmoAudioProperties.h"
#include <dmo.h>
#include <uuids.h>

// Taken from Kinect SDK sample
class CStaticMediaBuffer : public IMediaBuffer {
public:
   CStaticMediaBuffer() {}
   CStaticMediaBuffer(BYTE *pData, ULONG ulSize, ULONG ulData) :
      m_pData(pData), m_ulSize(ulSize), m_ulData(ulData) {}
   STDMETHODIMP_(ULONG) AddRef() { return 2; }
   STDMETHODIMP_(ULONG) Release() { return 1; }
   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
      if (riid == IID_IUnknown) {
         AddRef();
         *ppv = (IUnknown*)this;
         return NOERROR;
      }
      else if (riid == IID_IMediaBuffer) {
         AddRef();
         *ppv = (IMediaBuffer*)this;
         return NOERROR;
      }
      else
         return E_NOINTERFACE;
   }
   STDMETHODIMP SetLength(DWORD ulLength) {m_ulData = ulLength; return NOERROR;}
   STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) {*pcbMaxLength = m_ulSize; return NOERROR;}
   STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength) {
      if (ppBuffer) *ppBuffer = m_pData;
      if (pcbLength) *pcbLength = m_ulData;
      return NOERROR;
   }
   void Init(BYTE *pData, ULONG ulSize, ULONG ulData) {
        m_pData = pData;
        m_ulSize = ulSize;
        m_ulData = ulData;
   }   
   ULONG GetDataSize() { return m_ulData; }
protected:
   BYTE *m_pData;
   ULONG m_ulSize;
   ULONG m_ulData;
};


class DmoAudioReader
{
public:
	static const DWORD SAMPLE_PER_SEC = 16000;
	static const DWORD BITS_PER_SAMPLE = 16;
	static const DWORD BYTES_PER_SAMPLE = BITS_PER_SAMPLE >> 3;
	static const DWORD BYTES_PER_SEC = SAMPLE_PER_SEC * BYTES_PER_SAMPLE;
	static const DWORD BYTES_PER_MILLISEC = BYTES_PER_SEC / 1000;

private:
	INuiSensor* m_pSensor;
	INuiAudioBeam* m_pAudioBeam;
	IMediaObject* m_pDmo;
	IPropertyStore* m_pPropertyStore;

	DmoAudioProperties* m_pAudioProperties;

	CStaticMediaBuffer m_mediaBuffer;

	DWORD m_byteBufferSize;
	BYTE* m_byteBuffer;

	REFERENCE_TIME m_timestampInNs;
	REFERENCE_TIME m_timeLengthInNs;
	BOOL m_hasMoreData;

	double m_beamAngle;
	double m_sourceAngle;
	double m_sourceAngleConfidence;

public:
	DmoAudioReader(INuiSensor* pSensor, DmoAudioProperties* pAudioProperties) :
		m_pSensor(pSensor), m_pAudioBeam(NULL), m_pDmo(NULL), m_pPropertyStore(NULL),
		m_pAudioProperties(pAudioProperties),
		m_byteBuffer(NULL),
		m_timestampInNs(0), m_timeLengthInNs(0), m_hasMoreData(0),
		m_beamAngle(0), m_sourceAngle(0), m_sourceAngleConfidence(0)
	{
		try {
			CHECK_HRESULT(CoInitialize(NULL));

			m_pSensor = pSensor;
			CHECK_HRESULT(m_pSensor->NuiGetAudioSource(&m_pAudioBeam));
			CHECK_HRESULT(m_pAudioBeam->QueryInterface(IID_IMediaObject, (void**)&m_pDmo));
			CHECK_HRESULT(m_pAudioBeam->QueryInterface(IID_IPropertyStore, (void**)&m_pPropertyStore));

			m_pAudioProperties->SetPropertyStore(m_pPropertyStore);
			m_pAudioProperties->Update();
			SetUpMediaType();
			SetUpMediaBuffer();
			CHECK_HRESULT(m_pDmo->AllocateStreamingResources());
		} catch (XnStatusException&) {
			CleanUp();
			throw;
		}
	}

	~DmoAudioReader()
	{
		CleanUp();
		CoUninitialize();
	}

	DWORD GetDataSize() { return m_mediaBuffer.GetDataSize(); }
	BYTE* GetData() { return m_byteBuffer; }

	REFERENCE_TIME GetTimestampInNs() { return m_timestampInNs; }
	REFERENCE_TIME GetTimeLengthInNs() { return m_timeLengthInNs; }
	BOOL HasMoreData() { return m_hasMoreData; }

	double GetBeamAngle() { return m_beamAngle; }
	double GetSourceAngle() { return m_sourceAngle; }
	double GetSourceAngleConfidence() { return m_sourceAngleConfidence; }
	void SetBeamAngle(double value) { m_pAudioBeam->SetBeam(value); }

	HRESULT Read()
	{
		m_mediaBuffer.Init(m_byteBuffer, m_byteBufferSize, 0);
		DMO_OUTPUT_DATA_BUFFER out;
		out.pBuffer = &m_mediaBuffer;
		out.rtTimelength = 0;
		out.rtTimestamp = 0;
		DWORD status;
		HRESULT hr = m_pDmo->ProcessOutput(0, 1, &out, &status);
		if (hr == S_OK && m_mediaBuffer.GetDataSize() != 0) {
			m_hasMoreData = !!(out.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);
			m_timestampInNs = out.rtTimestamp;
			m_timeLengthInNs = out.rtTimelength;
			CHECK_HRESULT(m_pAudioBeam->GetBeam(&m_beamAngle));
			CHECK_HRESULT(m_pAudioBeam->GetPosition(&m_sourceAngle, &m_sourceAngleConfidence));
		}
		return hr;
	}

private:
	void CleanUp()
	{
		if (m_byteBuffer) delete [] m_byteBuffer;
		if (m_pPropertyStore) m_pPropertyStore->Release();
		if (m_pDmo) m_pDmo->Release();
		if (m_pAudioBeam) m_pAudioBeam->Release();
	}

	void SetUpMediaType()
	{
		WAVEFORMATEX wf = { WAVE_FORMAT_PCM, 1, SAMPLE_PER_SEC, BYTES_PER_SEC, BYTES_PER_SAMPLE, BITS_PER_SAMPLE, 0 };
		DMO_MEDIA_TYPE mt;
		MoInitMediaType(&mt, sizeof(wf));
		mt.majortype = MEDIATYPE_Audio;
		mt.subtype = MEDIASUBTYPE_PCM;
		mt.lSampleSize = 0;
		mt.bFixedSizeSamples = TRUE;
		mt.bTemporalCompression = FALSE;
		mt.formattype = FORMAT_WaveFormatEx;
		memcpy(mt.pbFormat, &wf, sizeof(wf));
		CHECK_HRESULT(m_pDmo->SetOutputType(0, &mt, 0));
		MoFreeMediaType(&mt);
	}

	void SetUpMediaBuffer()
	{
		m_byteBufferSize = SAMPLE_PER_SEC * BYTES_PER_SAMPLE; // 1 sec is more than enough
		m_byteBuffer = new BYTE[m_byteBufferSize];
	}

};
