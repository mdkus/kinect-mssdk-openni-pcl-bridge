#include "MSRKinectDepthGenerator.h"

MSRKinectDepthGenerator::MSRKinectDepthGenerator()
{
}

MSRKinectDepthGenerator::~MSRKinectDepthGenerator()
{
}

XnBool MSRKinectDepthGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return
		SuperClass::IsCapabilitySupported(strCapabilityName) ||
		strcmp(strCapabilityName, XN_CAPABILITY_ALTERNATIVE_VIEW_POINT) == 0;
}

xn::ModuleAlternativeViewPointInterface* MSRKinectDepthGenerator::GetAlternativeViewPointInterface()
{
	return this;
}

XnStatus MSRKinectDepthGenerator::UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
{
	// todo flexible resolution

	assert(lockedRect.Pitch == 320 * sizeof(USHORT));

	const USHORT* sp = data;
	XnDepthPixel* dp = m_pBuffer;

	if (!m_pReader->IsCalibrateViewPoint()) {
		for (XnUInt y = 0; y < 240; y++) {
			for (XnUInt x = 0; x < 320; x++) {
				processPixel(sp, dp);
				sp++;
				dp += 2;
			}
			dp += 640;
		}
	} else {
		memset(m_pBuffer, 0, 640*480);

		for (int y = 0; y < 240; y++) {
			for (int x = 0; x < 320; x++) {
				LONG ix, iy;
				NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, *sp &  ~NUI_IMAGE_PLAYER_INDEX_MASK, &ix, &iy);
				if (ix >= 0 && ix < 639 && iy >= 0 && iy < 479) {
					processPixel(sp, m_pBuffer + iy * 640 + ix);
				}
				sp++;
			}
		}
	}

	return XN_STATUS_OK;
}

XnDepthPixel* MSRKinectDepthGenerator::GetDepthMap()
{
	return m_pBuffer;
}

XnDepthPixel MSRKinectDepthGenerator::GetDeviceMaxDepth()
{
	return NUI_IMAGE_DEPTH_MAXIMUM >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
}

void MSRKinectDepthGenerator::GetFieldOfView(XnFieldOfView& fov)
{
	// empirical value
	fov.fHFOV = 1.0145;
	fov.fVFOV = 0.7898;
}

XnStatus MSRKinectDepthGenerator::RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	// ignore
	return XN_STATUS_OK;
}

void MSRKinectDepthGenerator::UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback)
{
	// ignore
}

