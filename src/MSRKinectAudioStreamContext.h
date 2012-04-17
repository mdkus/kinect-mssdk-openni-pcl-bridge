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

#include "base.h"
#include "MSRKinectFrameContextBase.h"
#include "DmoAudioReader.h"
#include <vector>

class MSRKinectAudioStreamContext : public MSRKinectFrameContextBase
{
private:
	typedef MSRKinectFrameContextBase SuperClass;
	typedef std::basic_string<BYTE> ByteBuffer;

protected:
	static const DWORD BUFFER_COUNT = 2;

	DmoAudioReader* m_pDmoAudio;

	ByteBuffer m_buffers[BUFFER_COUNT];
	DWORD m_bufferSize;
	DWORD m_currentBufferIndex;

	BYTE* m_data;
	DWORD m_dataSize;

	double m_beamAngle;
	double m_sourceAngle;
	double m_sourceAngleConfidence;

public:
	MSRKinectAudioStreamContext(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		SuperClass(pRequirement, hNextFrameEvent),
		m_pDmoAudio(NULL),
		m_bufferSize(DmoAudioReader::SAMPLE_PER_SEC * DmoAudioReader::BYTES_PER_SAMPLE), // 1 sec
		m_currentBufferIndex(0),
		m_data(NULL), m_dataSize(0),
		m_beamAngle(0), m_sourceAngle(0), m_sourceAngleConfidence(0)
	{
		for (int i = 0; i < BUFFER_COUNT; i++) {
			m_buffers[i].reserve(m_bufferSize);
		}
	}

	HRESULT GetNextFrame()
	{
		HRESULT hr;
		do {
			hr = m_pDmoAudio->Read();

			if (hr == S_OK) {
				LockFrame();
				ByteBuffer& buf = m_buffers[m_currentBufferIndex];
				ByteBuffer::size_type room = buf.capacity() - buf.size();
				ByteBuffer::size_type copySize = min(room, m_pDmoAudio->GetDataSize());
				buf.append(m_pDmoAudio->GetData(), copySize);
				UnlockFrame();
			}
		} while (hr == S_OK && m_pDmoAudio->HasMoreData());

		return hr;
	}

	void SwapBuffer()
	{
		m_nFrameID++;
		m_lTimestamp = m_pDmoAudio->GetTimestamp();

		
		ByteBuffer& buf = m_buffers[m_currentBufferIndex];
		m_dataSize = buf.length();
		m_data = const_cast<BYTE*>(buf.data());

		m_beamAngle = m_pDmoAudio->GetBeamAngle();
		m_sourceAngle = m_pDmoAudio->GetSourceAngle();
		m_sourceAngleConfidence = m_pDmoAudio->GetSourceAngleConfidence();

		m_currentBufferIndex = (m_currentBufferIndex + 1 ) % BUFFER_COUNT;
		m_buffers[m_currentBufferIndex].clear();
	}

	BYTE* GetData() { return m_data; }
	DWORD GetDataSize() { return m_dataSize; }
	double GetBeamAngle() { return m_beamAngle; }
	double GetSourceAngle() { return m_sourceAngle; }
	double GetSourceAngleConfidence() { return m_sourceAngleConfidence; }

protected:
	void SetUpDmoAudio() {
		m_pDmoAudio = new DmoAudioReader(m_pRequirement->GetSensor());
	}
};

