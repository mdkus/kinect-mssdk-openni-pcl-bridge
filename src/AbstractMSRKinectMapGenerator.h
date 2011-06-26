#pragma once
#include "base.h"
#include "AbstractMSRKinectFrameGenerator.h"

template <class ParentModuleMapGeneratorClass, class SourcePixelType, class TargetPixelType, NUI_IMAGE_TYPE eImageType>
class AbstractMSRKinectMapGenerator : public virtual AbstractMSRKinectFrameGenerator<ParentModuleMapGeneratorClass, SourcePixelType, TargetPixelType, eImageType>
{
protected:
	AbstractMSRKinectMapGenerator() {}

public:
	// MapGenerator methods

	virtual XnUInt32 GetSupportedMapOutputModesCount()
	{
		return 1; // 640x480x30fps only
	}

	virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
	{
		if (nCount < 1) {
			return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
		}
		getSupportedMapOutputMode(&aModes[0]);
		return XN_STATUS_OK;
	}

	void getSupportedMapOutputMode(XnMapOutputMode* pMode)
	{
		pMode->nXRes = X_RES;
		pMode->nYRes = Y_RES;
		pMode->nFPS = FPS;
	}

	virtual XnStatus SetMapOutputMode(const XnMapOutputMode& mode)
	{
		XnMapOutputMode supportedMode;
		getSupportedMapOutputMode(&supportedMode);
		if (supportedMode.nXRes == mode.nXRes && supportedMode.nYRes == mode.nYRes && supportedMode.nFPS == mode.nFPS) {
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_BAD_PARAM;
		}
	}

	virtual XnStatus GetMapOutputMode(XnMapOutputMode& mode)
	{
		getSupportedMapOutputMode(&mode);
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		// ignore, maybe someday
		return XN_STATUS_OK;
	}

	virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
	{
		// ignore, maybe someday
	}
};
