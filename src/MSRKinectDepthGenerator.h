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
#include "MSRKinectDepthGeneratorBase.h"
#include "MSRKinectMirrorCap.h"
#include "MSRKinectAlternativeViewPointCap.h"
#include "custom_properties.h"

class MSRKinectDepthGeneratorDepthPixelProcessor
{
public:
	void Process(USHORT d, XnDepthPixel* dp, XnInt32 ratio)
	{
		d >>= NUI_IMAGE_PLAYER_INDEX_SHIFT;
		*dp = d;
		if (ratio == 2) {
			*(dp+1) = *(dp+640) = *(dp+641) = d;
		}
	}
};

class MSRKinectDepthGenerator :
	public MSRKinectDepthGeneratorBase<xn::ModuleDepthGenerator, MSRKinectDepthGeneratorDepthPixelProcessor>,
	public virtual MSRKinectMirrorCap,
	public virtual MSRKinectAlternativeViewPointCap,
	public virtual ModuleExtendedSerializationInterface
{
private:
	typedef MSRKinectDepthGeneratorBase SuperClass;

public:
	MSRKinectDepthGenerator() : SuperClass(XN_NODE_TYPE_DEPTH, TRUE)
	{
	}

	virtual ~MSRKinectDepthGenerator() {}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			SuperClass::IsCapabilitySupported(strCapabilityName) ||
			MSRKinectMirrorCap::IsCapabilitySupported(strCapabilityName) ||
			MSRKinectAlternativeViewPointCap::IsCapabilitySupported(strCapabilityName) ||
			streq(strCapabilityName, XN_CAPABILITY_EXTENDED_SERIALIZATION);
	}

	// DepthGenerator methods
	virtual XnDepthPixel* GetDepthMap()
	{
		SetUpBuffer();
		return m_pBuffer;
	}

	virtual XnDepthPixel GetDeviceMaxDepth() { return NUI_IMAGE_DEPTH_MAXIMUM >> NUI_IMAGE_PLAYER_INDEX_SHIFT; }

	virtual void GetFieldOfView(XnFieldOfView& fov)
	{
		// empirical value
		fov.fHFOV = 1.0145;
		fov.fVFOV = 0.7898;
	}

	virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; } // ignore
	virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback) {} // ignore

	MSRKinectAlternativeViewPointCap_IMPL(m_pReader);
	MSRKinectMirrorCap_IMPL(m_pReader);

	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		// sorry for using downcast!
		if (streq(strName, PROP_DEPTH_DISTINCT_OVERFLOW_DEPTH_VALUES)) {
			nValue = ((MSRKinectDepthImageStreamReader*) m_pReader)->GetDistinctOverflowDepthValues();
			return XN_STATUS_OK;
		} else if (streq(strName, PROP_DEPTH_NEAR_MODE)) {
			nValue = ((MSRKinectDepthImageStreamReader*) m_pReader)->GetNearMode();
			return XN_STATUS_OK;
		} else {
			return SuperClass::GetIntProperty(strName, nValue);
		}
	}

	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		// sorry for using downcast!
		if (streq(strName, PROP_DEPTH_DISTINCT_OVERFLOW_DEPTH_VALUES)) {
			((MSRKinectDepthImageStreamReader*) m_pReader)->SetDistinctOverflowDepthValues(!!nValue);
			OnNodeIntPropChanged(PROP_DEPTH_DISTINCT_OVERFLOW_DEPTH_VALUES, !!nValue);
			return XN_STATUS_OK;
		} else if (streq(strName, PROP_DEPTH_NEAR_MODE)) {
			((MSRKinectDepthImageStreamReader*) m_pReader)->SetNearMode(!!nValue);
			OnNodeIntPropChanged(PROP_DEPTH_NEAR_MODE, !!nValue);
			return XN_STATUS_OK;
		} else {
			return SuperClass::SetIntProperty(strName, nValue);
		}
	}

	// ModuleExtendedSerializationInterface accessor
	virtual ModuleExtendedSerializationInterface* GetExtendedSerializationInterface() { return this; }

	// ModuleExtendedSerializationInterface implementation
	virtual XnStatus NotifyExState(XnNodeNotifications *pNotifications, void *pCookie)
	{
		RegisterNodeNotifications(pNotifications, pCookie);
		try {
			CHECK_XN_STATUS(OnNodeIntPropChanged(PROP_DEPTH_NEAR_MODE,
				((MSRKinectDepthImageStreamReader*) m_pReader)->GetNearMode()));
			CHECK_XN_STATUS(OnNodeIntPropChanged(PROP_DEPTH_DISTINCT_OVERFLOW_DEPTH_VALUES,
				((MSRKinectDepthImageStreamReader*) m_pReader)->GetDistinctOverflowDepthValues()));
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	virtual void UnregisterExNotifications()
	{
		UnregisterNodeNotifications();
	}

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		return UpdateDepthData(MSRKinectDepthGeneratorDepthPixelProcessor(), pFrame, data, lockedRect);
	}
};
