#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"

class MSRKinectImageGenerator : public virtual AbstractMSRKinectMapGenerator<xn::ModuleImageGenerator, DWORD, XnRGB24Pixel, NUI_IMAGE_TYPE_COLOR>
{
public:
	MSRKinectImageGenerator();
	virtual ~MSRKinectImageGenerator();

	// ImageGenerator methods
	virtual XnUInt8* GetImageMap();
	virtual XnBool IsPixelFormatSupported(XnPixelFormat Format);
	virtual XnStatus SetPixelFormat(XnPixelFormat Format);
	virtual XnPixelFormat GetPixelFormat();
	virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback);

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const DWORD* data, const KINECT_LOCKED_RECT& lockedRect);
};
