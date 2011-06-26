#pragma once
#include "base.h"
#include "MSRKinectFrameReader.h"

#define AbstractMSRKinectGeneratorControl_IMPL(pReader) \
public: \
	virtual XnBool IsGenerating() \
	{ \
		return pReader->IsRunning(); \
	} \
	\
	virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) \
	{ \
		return pReader->GetGeneratingEvent()->Register(handler, pCookie, &hCallback); \
	} \
	\
	virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback) \
	{ \
		pReader->GetGeneratingEvent()->Unregister(hCallback); \
	}

#define ActiveMSRKinectGeneratorControl_IMPL(pReader) \
	AbstractMSRKinectGeneratorControl_IMPL(pReader) \
	virtual XnStatus StartGenerating() \
	{ \
		try { \
			CHECK_XN_STATUS(pReader->Start()); \
			return XN_STATUS_OK; \
		} catch (XnStatusException& e) { \
			return e.nStatus; \
		} \
	} \
	\
	virtual void StopGenerating() \
	{ \
		pReader->Stop(); \
	}

#define PassiveMSRKinectGeneratorControl_IMPL(pReader) \
	AbstractMSRKinectGeneratorControl_IMPL(pReader) \
	virtual XnStatus StartGenerating() \
	{ \
		return XN_STATUS_INVALID_OPERATION; \
	} \
	\
	virtual void StopGenerating() \
	{ \
	}
