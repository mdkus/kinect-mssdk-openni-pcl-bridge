#pragma once
#include "base.h"

class MSRKinectRequirement
{
private:
	DWORD m_nInitFlags;
	NUI_IMAGE_RESOLUTION m_colorImageResolution;

public:
	MSRKinectRequirement() : m_nInitFlags(0), m_colorImageResolution(NUI_IMAGE_RESOLUTION_INVALID), m_depthImageResolution(NUI_IMAGE_RESOLUTION_INVALID)
	{
	}

	void AddRequirement(XnPredefinedProductionNodeType nodeType, const XnMapOutputMode& outputMode)
	{
		switch (nodeType) {
		case XN_NODE_TYPE_IMAGE:
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;

			// Lastly specified resolution wins
			if (outputMode.nXRes == 640 && outputMode.nYRes == 480) {
				m_colorImageResolution = NUI_IMAGE_RESOLUTION_640x480;
			} else if (outputMode.nXRes ==  1280 && outputMode.nYRes == 1024) {
				m_colorImageResolution = NUI_IMAGE_RESOLUTION_1280x1024;
			}
			break;

		case XN_NODE_TYPE_DEPTH:
			
			// Can we open both for NUI_IMAGE_TYPE_DEPTH and NUI_IMAGE_TYPE_COLOR_IN_DEPTH_SPACE?
			// Assume we cannot or should not.
			if (!(m_nInitFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX)) {
				m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
			}

			// Ignore the resolution
			break;

		case XN_NODE_TYPE_USER:
			// Assume we cannot or should not open NUI_INITIALIZE_FLAG_USES_DEPTH at the same time
			m_nInitFlags &= ~NUI_INITIALIZE_FLAG_USES_DEPTH;
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
			break;
		}
	}
}
