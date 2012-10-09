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
#include "MSRKinectFrameReader.h"
#include "MSRKinectImageStreamContext.h"
#include "ImageConfiguration.h"
#include "MSRKinectRequirement.h"

class MSRKinectImageStreamReader :
	public MSRKinectFrameReader<MSRKinectImageStreamContext>
{
private:
	typedef MSRKinectFrameReader<MSRKinectImageStreamContext> SuperClass;

public:
	MSRKinectImageStreamReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		SuperClass(pRequirement, hNextFrameEvent, 1000)
	{
	}

	void SetOutputMode(XnPredefinedProductionNodeType nodeType, const XnMapOutputMode& mode)
	{
		m_pRequirement->AddRequirement(nodeType, mode);
	}

protected:
	virtual void Setup()
	{
		if (!m_hStreamHandle) {
			m_pRequirement->DoInitialize();
			SetupStreamInfo();
			CHECK_HRESULT(GetSensor()->NuiImageStreamOpen(m_eImageType, m_eImageResolution, 0, 2, m_hNextFrameEvent, &m_hStreamHandle));
			PostStreamOpen();
		}
	}

	virtual void SetupStreamInfo() = 0;
	virtual void PostStreamOpen() {}

};

class MSRKinectColorImageStreamReader : public MSRKinectImageStreamReader
{
public:
	MSRKinectColorImageStreamReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		MSRKinectImageStreamReader(pRequirement, hNextFrameEvent)
	{
	}

protected:
	virtual void SetupStreamInfo()
	{
		m_eImageType = m_pRequirement->GetColorImageType();
		m_eImageResolution = m_pRequirement->GetColorImageResolution();
	}
};

class MSRKinectDepthImageStreamReader : public MSRKinectImageStreamReader
{
private:
	BOOL m_nearMode;
	BOOL m_distinctOverflowDepthValues;

public:
	MSRKinectDepthImageStreamReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		MSRKinectImageStreamReader(pRequirement, hNextFrameEvent),
		m_nearMode(FALSE), m_distinctOverflowDepthValues(FALSE)
	{
	}

	void SetNearMode(BOOL value)
	{
		m_nearMode = value;
		UpdateImageFrameFlags();
	}

	void SetDistinctOverflowDepthValues(BOOL value)
	{
		m_distinctOverflowDepthValues = value;
		UpdateImageFrameFlags();
	}

	BOOL GetNearMode()
	{
		return m_nearMode;
	}

	BOOL GetDistinctOverflowDepthValues()
	{
		return m_distinctOverflowDepthValues;
	}

protected:
	virtual void SetupStreamInfo()
	{
		m_eImageType = m_pRequirement->GetDepthImageType();
		m_eImageResolution = m_pRequirement->GetDepthImageResolution();
	}

	virtual void PostStreamOpen()
	{
		UpdateImageFrameFlags();
	}

	void UpdateImageFrameFlags()
	{
		if (m_hStreamHandle) {
			DWORD flags;
			CHECK_HRESULT(GetSensor()->NuiImageStreamGetImageFrameFlags(m_hStreamHandle, &flags));
			flags = changeBit<DWORD>(flags, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE, m_nearMode);
			flags = changeBit<DWORD>(flags, NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES, m_distinctOverflowDepthValues);
			HRESULT hr = GetSensor()->NuiImageStreamSetImageFrameFlags(m_hStreamHandle, flags);
			if (hr == E_NUI_HARDWARE_FEATURE_UNAVAILABLE) {
				// Kinect for Xbox360 does not support some features
				puts("The requested feature is not supported by the device. The request is ignored.\n"); // TODO log
			} else {
				CHECK_HRESULT(hr);
			}
		} else {
			// not initialized yet
		}
	}
};
