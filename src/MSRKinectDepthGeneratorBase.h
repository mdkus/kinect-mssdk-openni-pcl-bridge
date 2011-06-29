#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"

template<class ParentMapGeneratorClass, class DepthPixelProcessor>
class MSRKinectDepthGeneratorBase :
	public virtual AbstractMSRKinectMapGenerator<ParentMapGeneratorClass, USHORT, XnDepthPixel>
{
private:
	typedef AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel> SuperClass;

public:
	MSRKinectDepthGeneratorBase()
	{
		static ImageConfiguration::Mode s_modes[] = {
			ImageConfiguration::Mode(640, 480, 30, NUI_IMAGE_RESOLUTION_320x240)
		};
		static ImageConfiguration::Desc s_desc(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, s_modes, 1);
		SetImageConfigurationDesc(&s_desc);
	}

	virtual ~MSRKinectDepthGeneratorBase() {}

protected:

	XnStatus UpdateDepthData(DepthPixelProcessor& proc, const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
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
					proc.Process(sp, dp);
					sp += step;
					dp += 2;
				}
				dp += 640;
			}
		} else {
			XnUInt32 xRes = GetXRes();
			XnUInt32 yRes = GetYRes();
			memset(m_pBuffer, 0, xRes * yRes);

			for (int y = 0; y < 240; y++) {
				sp = data + y * 320 + (step < 0 ? 320-1 : 0);
				for (int x = 0; x < 320; x++) {
					LONG ix, iy;
					NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, *sp &  ~NUI_IMAGE_PLAYER_INDEX_MASK, &ix, &iy);
					if (ix >= 0 && ix < LONG(xRes-1) && iy >= 0 && iy < LONG(yRes-1)) {
						proc.Process(sp, m_pBuffer + iy * xRes + ix);
					}
					sp += step;
				}
			}
		}

		return XN_STATUS_OK;
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect) = 0;
};
