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
		const USHORT* sp = data;
		XnDepthPixel* dp = m_pBuffer;

		XnUInt32 destXRes = GetXRes();
		XnUInt32 destYRes = GetYRes();
		DWORD sourceXRes;
		DWORD sourceYRes;
		NuiImageResolutionToSize(pFrame->eResolution, sourceXRes, sourceYRes);
		int ratio = destXRes / sourceXRes;
		assert(ratio != 0);
		int step = m_pReader->GetMirrorFactor();

		BOOL hasPlayerIndex = m_pReader->GetImageType() == NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX;

		if (!m_pReader->IsCalibrateViewPoint()) {
			for (XnUInt32 y = 0; y < sourceYRes; y++) {
				sp = data + y * sourceXRes + (step < 0 ? sourceXRes-1 : 0);
				for (XnUInt32 x = 0; x < sourceXRes; x++) {
					USHORT d = hasPlayerIndex ? (*sp) : (*sp << 3);
					proc.Process(d, dp, ratio);
					sp += step;
					dp += ratio;
				}
				dp += destXRes * (ratio-1);
			}
		} else {
			// NuiImageGetColorPixelCoordinatesFromDepthPixel only supports Image@640x480 and Depth@320x240

			assert(destXRes == 640 && destYRes == 480);
			memset(m_pBuffer, 0, destXRes * destYRes);

			const XnUInt32 convXRes = 320;
			const XnUInt32 convYRes = 240;
			const XnUInt32 convRatio = sourceXRes / convXRes;
			assert(convRatio != 0);

			for (XnUInt32 y = 0; y < convYRes; y++) {
				sp = data + y * sourceXRes * convRatio + (step < 0 ? sourceXRes-1 : 0);
				for (XnUInt32 x = 0; x < convXRes; x++) {
					LONG ix, iy;
					USHORT d = hasPlayerIndex ? (*sp) : (*sp << 3);
					NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, (d & ~NUI_IMAGE_PLAYER_INDEX_MASK), &ix, &iy);
					if (ix >= 0 && ix < LONG(destXRes-1) && iy >= 0 && iy < LONG(destYRes-1)) {
						proc.Process(d, m_pBuffer + iy * destXRes + ix, 2);
					}
					sp += convRatio * step;
				}
			}
		}

		return XN_STATUS_OK;
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect) = 0;
};
