#pragma once
#include "base.h"
#include "MSRKinectFrameContextBase.h"

class MSRKinectAlternativeViewPointCap : public virtual xn::ModuleAlternativeViewPointInterface
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
		return strcmp(other.GetInfo().GetDescription().strName, "MSRKinectImageGenerator") == 0; \
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
		return pContext->GetCalibrateViewPointChangeEvent().Register(handler, pCookie, &hCallback); \
	} \
	\
	virtual void UnregisterFromViewPointChange(XnCallbackHandle hCallback) \
	{ \
		pContext->GetCalibrateViewPointChangeEvent().Unregister(hCallback); \
	}
