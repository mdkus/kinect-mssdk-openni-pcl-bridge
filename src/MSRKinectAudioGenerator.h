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
#include "AbstractMultiThreadFrameGenerator.h"
#include "MSRKinectAudioStreamReader.h"
#include "AudioConfiguration.h"
#include "custom_properties.h"

// Initial and naive implementation
class MSRKinectAudioGenerator :
	public AbstractMultiThreadFrameGenerator<xn::ModuleAudioGenerator, MSRKinectAudioStreamReader>
{
private:
	typedef AbstractMultiThreadFrameGenerator<xn::ModuleAudioGenerator, MSRKinectAudioStreamReader> SuperClass;

private:
	static AudioConfiguration::Desc* GetAudioConfigDesc()
	{
		static AudioConfiguration::Mode s_modes[] = {
			AudioConfiguration::Mode(16, 1, 16000)
		};
		static AudioConfiguration::Desc s_desc(s_modes, 1);
		return &s_desc;
	}

private:
	AudioConfiguration m_audioConfig;
	XnUInt32 m_dataSize;
	XnUChar* m_data;

public:
	MSRKinectAudioGenerator() : SuperClass(TRUE), m_audioConfig(GetAudioConfigDesc()), m_dataSize(0), m_data(NULL)
	{
		MSRKinectManager* pMan = MSRKinectManager::GetInstance();
		m_pReader = pMan->GetAudioStreamManager()->GetReader();
	}

	~MSRKinectAudioGenerator()
	{
	}

	// Generator methods

	XnStatus UpdateData()
	{
		m_pReader->LockFrame();
		m_dataSize = m_pReader->GetDataSize();
		m_data = m_pReader->GetData();
		m_pReader->SwapBuffer();
		m_bNewDataAvailable = FALSE;
		m_pReader->UnlockFrame();
		return XN_STATUS_OK;
	}

	XnUInt32 GetDataSize()
	{
		return m_dataSize;
	}

	XnUChar* GetAudioBuffer()
	{
		return m_data;
	}

	XnUInt32 GetSupportedWaveOutputModesCount()
	{
		return m_audioConfig.GetNumberOfSupportedModes();
	}

	XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount)
	{
		return m_audioConfig.GetSupportedRawModes(aSupportedModes, nCount);
	}

	XnStatus SetWaveOutputMode(const XnWaveOutputMode& outputMode)
	{
		// Only one mode is supported. There is nothing to re-initialize.
		return m_audioConfig.SelectRawMode(outputMode);
	}

	XnStatus GetWaveOutputMode(XnWaveOutputMode& outputMode)
	{
		outputMode = m_audioConfig.GetSelectedMode()->outputMode;
		return XN_STATUS_OK;
	}

	XnStatus RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		// ignore, maybe someday
		return XN_STATUS_OK;
	}

	void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
	{
		// ignore, maybe someday
	}

	// Custom properties

	XnStatus GetRealProperty(const XnChar* strName, XnDouble& dValue) const
	{
		if (strcmp(strName, PROP_AUDIO_BEAM_ANGLE) == 0) {
			dValue = m_pReader->GetBeamAngle();
			return XN_STATUS_OK;
		} else if (strcmp(strName, PROP_AUDIO_SOURCE_ANGLE) == 0) {
			dValue = m_pReader->GetSourceAngle();
			return XN_STATUS_OK;
		} else if (strcmp(strName, PROP_AUDIO_SOURCE_ANGLE_CONFIDENCE) == 0) {
			dValue = m_pReader->GetSourceAngleConfidence();
			return XN_STATUS_OK;
		} else {
			return SuperClass::GetRealProperty(strName, dValue);
		}
	}

};
