#pragma once
#include "base.h"

class MSRKinectRequirement
{
private:
	DWORD m_nInitFlags;
	NUI_IMAGE_RESOLUTION m_colorImageResolution;
	NUI_IMAGE_RESOLUTION m_depthImageResolution;
	BOOL m_bInitialized;

public:
	MSRKinectRequirement() :
		m_nInitFlags(0),
		m_colorImageResolution(NUI_IMAGE_RESOLUTION_INVALID),
		m_depthImageResolution(NUI_IMAGE_RESOLUTION_INVALID),
		m_bInitialized(FALSE)
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
			// We cannot open both NUI_IMAGE_TYPE_DEPTH and NUI_IMAGE_TYPE_COLOR_IN_DEPTH_SPACE at the sametime.
			if (!(m_nInitFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX)) {
				m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
			}

			// Lastly specified resolution wins
			if (outputMode.nXRes == 640 && outputMode.nYRes == 480) {
				m_depthImageResolution = NUI_IMAGE_RESOLUTION_640x480;
			} else if (outputMode.nXRes ==  320 && outputMode.nYRes == 240) {
				m_depthImageResolution = NUI_IMAGE_RESOLUTION_320x240;
			}
			break;

		case XN_NODE_TYPE_USER:
			// Assume we cannot or should not open NUI_INITIALIZE_FLAG_USES_DEPTH at the same time
			m_nInitFlags &= ~NUI_INITIALIZE_FLAG_USES_DEPTH;
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;

			// Ignore the resolution
			break;
		}
	}

	void AddCapabilityRequirement(const char* strCapability)
	{
		if (strcmp(strCapability, XN_CAPABILITY_SKELETON) == 0) {
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_SKELETON;
		}
	}

	DWORD GetInitiFlags() const
	{
		return m_nInitFlags;
	}

	NUI_IMAGE_RESOLUTION GetColorImageResolution() const
	{
		return m_colorImageResolution;
	}

	BOOL IsUserNodeRequired() const
	{
		return m_nInitFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
	}

	NUI_IMAGE_RESOLUTION GetDepthImageResolution() const
	{
		return IsUserNodeRequired() ? NUI_IMAGE_RESOLUTION_320x240 : m_depthImageResolution;
	}

	NUI_IMAGE_TYPE GetDepthImageType() const
	{
		return IsUserNodeRequired() ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH;
	}

	void DoInitialize() // throws XnStatusException
	{
		if (m_bInitialized) return;

		CHECK_HRESULT(NuiInitialize(m_nInitFlags));
		m_bInitialized = TRUE;
	}

};
