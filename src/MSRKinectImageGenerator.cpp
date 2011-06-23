#include "MSRKinectImageGenerator.h"

MSRKinectImageGenerator::MSRKinectImageGenerator()
{
}

MSRKinectImageGenerator::~MSRKinectImageGenerator()
{
}

XnStatus MSRKinectImageGenerator::UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const DWORD* data, const KINECT_LOCKED_RECT& lockedRect)
{
	// todo support mirror
	// todo flexible resolution

	assert(lockedRect.Pitch == 640 * sizeof(DWORD));

	const DWORD* sp = data;
	XnRGB24Pixel* dp = m_pBuffer;

	for (XnUInt y = 0; y < 480; y++) {
		for (XnUInt x = 0; x < 640; x++) {
			dp->nRed = XnUInt8(*sp >> 16);
			dp->nGreen = XnUInt8(*sp >> 8);
			dp->nBlue = XnUInt8(*sp >> 0);

			sp++;
			dp++;
		}
	}

	return XN_STATUS_OK;
}

XnUInt8* MSRKinectImageGenerator::GetImageMap()
{
	return (XnUInt8*) m_pBuffer;
}

XnBool MSRKinectImageGenerator::IsPixelFormatSupported(XnPixelFormat format)
{
	return format == XN_PIXEL_FORMAT_RGB24;
}

XnStatus MSRKinectImageGenerator::SetPixelFormat(XnPixelFormat format)
{
	return (format == XN_PIXEL_FORMAT_RGB24) ? XN_STATUS_OK : XN_STATUS_BAD_PARAM;
}

XnPixelFormat MSRKinectImageGenerator::GetPixelFormat()
{
	return XN_PIXEL_FORMAT_RGB24;
}

XnStatus MSRKinectImageGenerator::RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	// ignore, maybe someday
	return XN_STATUS_OK;
}

void MSRKinectImageGenerator::UnregisterFromPixelFormatChange(XnCallbackHandle hCallback)
{
	// ignore, maybe someday
}
