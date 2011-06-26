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
	virtual XnUInt8* GetImageMap()
	{
		return (XnUInt8*) m_pBuffer;
	}

	virtual XnBool IsPixelFormatSupported(XnPixelFormat format)
	{
		return format == XN_PIXEL_FORMAT_RGB24;
	}

	virtual XnStatus SetPixelFormat(XnPixelFormat format) {
		return (format == XN_PIXEL_FORMAT_RGB24) ? XN_STATUS_OK : XN_STATUS_BAD_PARAM;
	}

	virtual XnPixelFormat GetPixelFormat()
	{
		return XN_PIXEL_FORMAT_RGB24;
	}

	virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; } // ignore
	virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback) {} // ignore

	MSRKinectMirrorCap_IMPL(m_pReader);
	ActiveMSRKinectGeneratorControl_IMPL(m_pReader);

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const DWORD* data, const KINECT_LOCKED_RECT& lockedRect)
	{
		// todo support mirror
		// todo flexible resolution

		assert(lockedRect.Pitch == 640 * sizeof(DWORD));

		const DWORD* sp = data;
		XnRGB24Pixel* dp = m_pBuffer;

		int step = m_pReader->GetMirrorFactor();
		for (XnUInt y = 0; y < 480; y++) {
			sp = data + y * 640 + (step < 0 ? 640-1 : 0);
			for (XnUInt x = 0; x < 640; x++) {
				dp->nRed = XnUInt8(*sp >> 16);
				dp->nGreen = XnUInt8(*sp >> 8);
				dp->nBlue = XnUInt8(*sp >> 0);

				sp += step;
				dp++;
			}
		}

		return XN_STATUS_OK;
	}

};
