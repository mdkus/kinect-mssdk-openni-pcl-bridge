#pragma once
#include "MSRKinectUserSkeletonGenerator.h"

class MSRKinectUserSkeletonGeneratorWithAutoElevation :
	public MSRKinectUserSkeletonGenerator
{
private:
	typedef MSRKinectUserSkeletonGenerator SuperClass;

private:
	LONGLONG m_liTimestamp;

public:
	MSRKinectUserSkeletonGeneratorWithAutoElevation() : m_liTimestamp(0)
	{
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		XnStatus result = SuperClass::UpdateImageData(pFrame, data, lockedRect);
		if (result != XN_STATUS_OK) return result;

		changeElevation();

		return XN_STATUS_OK;
	}

private:
	void changeElevation()
	{
		if (abs(m_skeletonFrame.liTimeStamp.QuadPart - m_liTimestamp) > 5000) {
			XnUserID userIDs[6];
			XnUInt16 users = 6;
			GetUsers(userIDs, users);
			if (users > 0) {
				NUI_SKELETON_DATA* sd = GetSkeletonData(userIDs[0]);
				DWORD clippedTop = sd->dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_TOP;
				DWORD clippedBottom = sd->dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_BOTTOM;
				LONG angle;
				NuiCameraElevationGetAngle(&angle);
				if (clippedTop && !clippedBottom) {
					angle += 5;
				} else if (clippedBottom && !clippedTop) {
					angle -= 5;
				}
				NuiCameraElevationSetAngle(angle);
			}
			m_liTimestamp = m_skeletonFrame.liTimeStamp.QuadPart;
		}
	}

};
