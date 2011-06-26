#pragma once
#include "base.h"
#include "MSRKinectStreamReader.h"

class MSRKinectStreamAlternativeViewPointCap : public virtual xn::ModuleAlternativeViewPointInterface
{
protected:
	virtual MSRKinectStreamContextBase* GetContextBase() = 0;

	virtual XnBool IsViewPointSupported(xn::ProductionNode& other)
	{
		return strcmp(other.GetInfo().GetDescription().strName, "MSRKinectImageGenerator") == 0;
	}

	virtual XnStatus SetViewPoint(xn::ProductionNode& other)
	{
		if (IsViewPointSupported(other)) {
			GetContextBase()->SetCalibrateViewPoint(TRUE);
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_BAD_PARAM;
		}
	}

	virtual XnBool IsViewPointAs(xn::ProductionNode& other)
	{
		return GetContextBase()->IsCalibrateViewPoint() && IsViewPointSupported(other);
	}

	virtual XnStatus ResetViewPoint()
	{
		GetContextBase()->SetCalibrateViewPoint(FALSE);
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return GetContextBase()->GetCalibrateViewPointChangeEvent().Register(handler, pCookie, &hCallback);
	}

	virtual void UnregisterFromViewPointChange(XnCallbackHandle hCallback)
	{
		GetContextBase()->GetCalibrateViewPointChangeEvent().Unregister(hCallback);
	}
};
