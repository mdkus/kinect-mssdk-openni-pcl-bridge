#pragma once
#include "base.h"
#include "MSRKinectFrameContextBase.h"

class MSRKinectMirrorCap : public virtual xn::ModuleMirrorInterface
{
protected:
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return strcmp(XN_CAPABILITY_MIRROR, strCapabilityName) == 0;
	}

};

#define MSRKinectMirrorCap_IMPL(pContext) \
public: \
	virtual xn::ModuleMirrorInterface* GetMirrorInterface() \
	{ \
		return this; \
	} \
	\
	virtual XnStatus SetMirror(XnBool bMirror) \
	{ \
		if (pContext->IsMirror() != bMirror) { \
			pContext->SetMirror(bMirror); \
		} \
		return XN_STATUS_OK; \
	} \
	\
	virtual XnBool IsMirrored() \
	{ \
		return pContext->IsMirror(); \
	} \
	\
	virtual XnStatus RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) \
	{ \
		return pContext->GetMirrorChangeEvent().Register(handler, pCookie, &hCallback); \
	} \
	\
	virtual void UnregisterFromMirrorChange(XnCallbackHandle hCallback) \
	{ \
		pContext->GetMirrorChangeEvent().Unregister(hCallback); \
	}
