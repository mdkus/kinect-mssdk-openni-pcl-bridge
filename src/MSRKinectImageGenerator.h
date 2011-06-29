#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"
#include "MSRKinectMirrorCap.h"
#include "MSRKinectGeneratorControls.h"

class MSRKinectImageGenerator :
	public virtual AbstractMSRKinectMapGenerator<xn::ModuleImageGenerator, DWORD, XnRGB24Pixel>,
	public virtual MSRKinectMirrorCap
{
public:
	MSRKinectImageGenerator()
	{
		static ImageConfiguration::Mode s_modes[] = {
			ImageConfiguration::Mode(640, 480, 30, NUI_IMAGE_RESOLUTION_640x480),
			ImageConfiguration::Mode(1280, 1024, 15, NUI_IMAGE_RESOLUTION_1280x1024)
		};
		static ImageConfiguration::Desc s_desc(NUI_IMAGE_TYPE_COLOR, s_modes, 2);
		SetImageConfigurationDesc(&s_desc);
	}

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
		assert(lockedRect.Pitch == GetXRes() * sizeof(DWORD));

		const DWORD* sp = data;
		XnRGB24Pixel* dp = m_pBuffer;

		XnUInt32 xRes = GetXRes();
		XnUInt32 yRes = GetYRes();
		int step = m_pReader->GetMirrorFactor();
		for (XnUInt y = 0; y < yRes; y++) {
			sp = data + y * xRes + (step < 0 ? xRes-1 : 0);
			for (XnUInt x = 0; x < xRes; x++) {
				DWORD c = *sp;
				dp->nRed = XnUInt8(c >> 16);
				dp->nGreen = XnUInt8(c >> 8);
				dp->nBlue = XnUInt8(c >> 0);

				sp += step;
				dp++;
			}
		}

		return XN_STATUS_OK;
	}

};
