#include "MSRKinectDepthGenerator.h"

MSRKinectDepthGenerator::MSRKinectDepthGenerator() : m_bCalibrateViewPoint(FALSE)
{
}

MSRKinectDepthGenerator::~MSRKinectDepthGenerator()
{
}

XnBool MSRKinectDepthGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return
		AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240>::IsCapabilitySupported(strCapabilityName) ||
		strcmp(strCapabilityName, XN_CAPABILITY_ALTERNATIVE_VIEW_POINT) == 0;
}

xn::ModuleAlternativeViewPointInterface* MSRKinectDepthGenerator::GetAlternativeViewPointInterface()
{
	return this;
}

XnStatus MSRKinectDepthGenerator::UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
{
	// todo support mirror
	// todo flexible resolution

	assert(lockedRect.Pitch == 320 * sizeof(USHORT));

	const USHORT* sp = data;
	XnDepthPixel* dp = m_pBuffer;

	if (!m_bCalibrateViewPoint) {
		for (XnUInt y = 0; y < 240; y++) {
			for (XnUInt x = 0; x < 320; x++) {
				*dp = *(dp+1) = *(dp+640) = *(dp+641) = (*sp >> NUI_IMAGE_PLAYER_INDEX_SHIFT);
				sp++;
				dp += 2;
			}
			dp += 640;
		}
	} else {
		memset(m_pBuffer, 0, 640*480);

		for (int y = 0; y < 240; y++) {
			for (int x = 0; x < 320; x++) {
				USHORT z = *sp >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
				LONG ix, iy;
				NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, z << NUI_IMAGE_PLAYER_INDEX_SHIFT, &ix, &iy);
				if (ix >= 0 && ix < 639 && iy >= 0 && iy < 479) {
					dp = m_pBuffer + iy * 640 + ix;
					*dp = *(dp+1) = *(dp+640) = *(dp+641) = z;
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

XnBool MSRKinectDepthGenerator::IsViewPointSupported(xn::ProductionNode& other)
{
	return strcmp(other.GetName(), "MSRKinectImageGenerator") == 0;
}

XnStatus MSRKinectDepthGenerator::SetViewPoint(xn::ProductionNode& other)
{
	if (!m_bCalibrateViewPoint) {
		m_bCalibrateViewPoint = TRUE;
		m_viewPointChangeEvent.Raise();
	}
	return XN_STATUS_OK;
}

XnBool MSRKinectDepthGenerator::IsViewPointAs(xn::ProductionNode& other)
{
	return m_bCalibrateViewPoint && IsViewPointSupported(other);
}

XnStatus MSRKinectDepthGenerator::ResetViewPoint()
{
	if (m_bCalibrateViewPoint) {
		m_bCalibrateViewPoint = FALSE;
		m_viewPointChangeEvent.Raise();
	}
	return XN_STATUS_OK;
}

XnStatus MSRKinectDepthGenerator::RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_viewPointChangeEvent.Register(handler, pCookie, &hCallback);
}

void MSRKinectDepthGenerator::UnregisterFromViewPointChange(XnCallbackHandle hCallback)
{
	m_viewPointChangeEvent.Unregister(hCallback);
}

