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
		try {
			if (streq(strName, PROP_AUDIO_BEAM_ANGLE)) {
				dValue = m_pReader->GetBeamAngle();
			} else if (streq(strName, PROP_AUDIO_SOURCE_ANGLE)) {
				dValue = m_pReader->GetSourceAngle();
			} else if (streq(strName, PROP_AUDIO_SOURCE_ANGLE_CONFIDENCE)) {
				dValue = m_pReader->GetSourceAngleConfidence();
			} else {
				return SuperClass::GetRealProperty(strName, dValue);
			}
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	XnStatus SetRealProperty(const XnChar* strName, XnDouble dValue)
	{
		try {
			if (streq(strName, PROP_AUDIO_BEAM_ANGLE)) {
				m_pReader->SetBeamAngle(dValue);
			} else {
				return SuperClass::SetRealProperty(strName, dValue);
			}
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		try {
			if (streq(strName, PROP_AUDIO_AUTOMATIC_GAIN_CONTROL)) {
				nValue = m_pReader->GetAudioProperties()->GetAutomaticGainControl();
			} else if (streq(strName, PROP_AUDIO_BEAM_ANGLE_MODE)) {
				nValue = m_pReader->GetAudioProperties()->GetBeamAngleMode();
			} else if (streq(strName, PROP_AUDIO_ECHO_CANCELLATION_MODE)) {
				nValue = m_pReader->GetAudioProperties()->GetEchoCancellationMode();
			} else if (streq(strName, PROP_AUDIO_CENTER_CLIPPING_MODE)) {
				nValue = m_pReader->GetAudioProperties()->GetCenterClippingMode();
			} else if (streq(strName, PROP_AUDIO_NOISE_SUPPRESSION)) {
				nValue = m_pReader->GetAudioProperties()->GetNoiseSuppression();
			} else if (streq(strName, PROP_AUDIO_BUFFER_SIZE)) {
				nValue = m_pReader->GetBufferSizeInMs();
			} else {
				return SuperClass::GetIntProperty(strName, nValue);
			}
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		try {
			int iValue = (int)nValue;
			if (streq(strName, PROP_AUDIO_AUTOMATIC_GAIN_CONTROL)) {
				m_pReader->GetAudioProperties()->SetAutomaticGainControl(iValue);
			} else if (streq(strName, PROP_AUDIO_BEAM_ANGLE_MODE)) {
				m_pReader->GetAudioProperties()->SetBeamAngleMode(iValue);
			} else if (streq(strName, PROP_AUDIO_ECHO_CANCELLATION_MODE)) {
				m_pReader->GetAudioProperties()->SetEchoCancellationMode(iValue);
			} else if (streq(strName, PROP_AUDIO_CENTER_CLIPPING_MODE)) {
				m_pReader->GetAudioProperties()->SetCenterClippingMode(iValue);
			} else if (streq(strName, PROP_AUDIO_NOISE_SUPPRESSION)) {
				m_pReader->GetAudioProperties()->SetNoiseSuppression(iValue);
			} else if (streq(strName, PROP_AUDIO_BUFFER_SIZE)) {
				m_pReader->SetBufferSizeInMs(iValue);
			} else {
				return SuperClass::GetIntProperty(strName, nValue);
			}
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

};
