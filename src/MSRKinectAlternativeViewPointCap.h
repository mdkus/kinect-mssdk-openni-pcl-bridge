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
#include "MSRKinectFrameContextBase.h"

class MSRKinectAlternativeViewPointCap :
	public virtual xn::ModuleAlternativeViewPointInterface
{
protected:
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return strcmp(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT, strCapabilityName) == 0;
	}

};

#define MSRKinectAlternativeViewPointCap_IMPL(pContext) \
public: \
	virtual xn::ModuleAlternativeViewPointInterface* GetAlternativeViewPointInterface() \
	{ \
		return this; \
	} \
	\
	virtual XnBool IsViewPointSupported(xn::ProductionNode& other) \
	{ \
		return \
			strcmp(other.GetInfo().GetDescription().strName, "MSRKinectImageGenerator") == 0 || \
			strcmp(other.GetInfo().GetDescription().strName, "KinectSDKImageGenerator") == 0 ; \
	} \
	\
	virtual XnStatus SetViewPoint(xn::ProductionNode& other) \
	{ \
		if (IsViewPointSupported(other)) { \
			pContext->SetCalibrateViewPoint(TRUE); \
			return XN_STATUS_OK; \
		} else { \
			return XN_STATUS_BAD_PARAM; \
		} \
	} \
	\
	virtual XnBool IsViewPointAs(xn::ProductionNode& other) \
	{ \
		return pContext->IsCalibrateViewPoint() && IsViewPointSupported(other); \
	} \
	\
	virtual XnStatus ResetViewPoint() \
	{ \
		pContext->SetCalibrateViewPoint(FALSE); \
		return XN_STATUS_OK; \
	} \
	\
	virtual XnStatus RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) \
	{ \
		return pContext->GetCalibrateViewPointChangeEvent()->Register(handler, pCookie, &hCallback); \
	} \
	\
	virtual void UnregisterFromViewPointChange(XnCallbackHandle hCallback) \
	{ \
		pContext->GetCalibrateViewPointChangeEvent()->Unregister(hCallback); \
	}
