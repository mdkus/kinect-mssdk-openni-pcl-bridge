#pragma once
#include "base.h"
#include "MSRKinectStreamReader.h"

class MSRKinectStreamMirrorCap : public virtual xn::ModuleMirrorInterface
{
protected:
	virtual MSRKinectStreamContextBase* GetContextBase() = 0;

	virtual XnStatus SetMirror(XnBool bMirror)
	{
		if (GetContextBase()->IsMirror() != bMirror) {
			GetContextBase()->SetMirror(bMirror);
		}
		return XN_STATUS_OK;
	}

	virtual XnBool IsMirrored()
	{
		return GetContextBase()->IsMirror();
	}

	virtual XnStatus RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return GetContextBase()->GetMirrorChangeEvent().Register(handler, pCookie, &hCallback);
	}

	virtual void UnregisterFromMirrorChange(XnCallbackHandle hCallback)
	{
		GetContextBase()->GetMirrorChangeEvent().Unregister(hCallback);
	}

};
