#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"
#include "MSRKinectMirrorCap.h"
#include "MSRKinectGeneratorControls.h"

class MSRKinectImageGenerator :
	public virtual AbstractMSRKinectMapGenerator<xn::ModuleImageGenerator, DWORD, XnRGB24Pixel, NUI_IMAGE_TYPE_COLOR>,
	public virtual MSRKinectMirrorCap
{
public:
	MSRKinectImageGenerator() {}
	virtual ~MSRKinectImageGenerator() {}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return MSRKinectMirrorCap::IsCapabilitySupported(strCapabilityName);
	}

	// ImageGenerator methods
	virtual XnUInt8* GetImageMap();
	virtual XnBool IsPixelFormatSupported(XnPixelFormat Format);
	virtual XnStatus SetPixelFormat(XnPixelFormat Format);
	virtual XnPixelFormat GetPixelFormat();
	virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback);

	MSRKinectMirrorCap_IMPL(m_pReader);
	ActiveMSRKinectGeneratorControl_IMPL(m_pReader);

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const DWORD* data, const KINECT_LOCKED_RECT& lockedRect);
};
