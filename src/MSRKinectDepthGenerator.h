#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"
#include "MSRKinectGeneratorControls.h"
#include "MSRKinectMirrorCap.h"
#include "MSRKinectAlternativeViewPointCap.h"

class MSRKinectDepthGenerator :
	public virtual AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX>,
	public virtual MSRKinectMirrorCap,
	public virtual MSRKinectAlternativeViewPointCap
{
private:
	typedef AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX> SuperClass;

public:
	MSRKinectDepthGenerator() {}
	virtual ~MSRKinectDepthGenerator() {}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			MSRKinectMirrorCap::IsCapabilitySupported(strCapabilityName) ||
			MSRKinectAlternativeViewPointCap::IsCapabilitySupported(strCapabilityName);
	}

	// DepthGenerator methods
	virtual XnDepthPixel* GetDepthMap() { return m_pBuffer; }

	virtual XnDepthPixel GetDeviceMaxDepth() { return NUI_IMAGE_DEPTH_MAXIMUM >> NUI_IMAGE_PLAYER_INDEX_SHIFT; }

	virtual void GetFieldOfView(XnFieldOfView& fov)
	{
		// empirical value
		fov.fHFOV = 1.0145;
		fov.fVFOV = 0.7898;
	}

	virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; } // ignore
	virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback) {} // ignore

	MSRKinectAlternativeViewPointCap_IMPL(m_pReader);
	MSRKinectMirrorCap_IMPL(m_pReader);
	ActiveMSRKinectGeneratorControl_IMPL(m_pReader);

protected:
	inline void processPixel(const USHORT* sp, XnDepthPixel* dp)
	{
		*dp = *(dp+1) = *(dp+640) = *(dp+641) = (*sp >> NUI_IMAGE_PLAYER_INDEX_SHIFT);
	}

	virtual XnStatus MSRKinectDepthGenerator::UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
	{
		// todo flexible resolution

		assert(lockedRect.Pitch == 320 * sizeof(USHORT));

		const USHORT* sp = data;
		XnDepthPixel* dp = m_pBuffer;

		int step = m_pReader->GetMirrorFactor();
		if (!m_pReader->IsCalibrateViewPoint()) {
			for (XnUInt y = 0; y < 240; y++) {
				sp = data + y * 320 + (step < 0 ? 320-1 : 0);
				for (XnUInt x = 0; x < 320; x++) {
					processPixel(sp, dp);
					sp += step;
					dp += 2;
				}
				dp += 640;
			}
		} else {
			memset(m_pBuffer, 0, 640*480);

			for (int y = 0; y < 240; y++) {
				sp = data + y * 320 + (step < 0 ? 320-1 : 0);
				for (int x = 0; x < 320; x++) {
					LONG ix, iy;
					NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, *sp &  ~NUI_IMAGE_PLAYER_INDEX_MASK, &ix, &iy);
					if (ix >= 0 && ix < 639 && iy >= 0 && iy < 479) {
						processPixel(sp, m_pBuffer + iy * 640 + ix);
					}
					sp += step;
				}
			}
		}

		return XN_STATUS_OK;
	}
};
