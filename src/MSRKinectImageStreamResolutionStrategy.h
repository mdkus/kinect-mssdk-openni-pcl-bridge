#pragma once
#include "base.h"

class MSRKinectImageStreamResolutionStrategy
{
public:
	virtual ~MSRKinectImageStreamResolutionStrategy() {}
	virtual XnStatus AddRequirement(NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution) = 0;
	virtual NUI_IMAGE_RESOLUTION GetOptimalResolution() = 0;
};

class AbstractMSRKinectImageStreamResolutionStrategy : public MSRKinectImageStreamResolutionStrategy
{
protected:
	NUI_IMAGE_RESOLUTION m_eOptimalResolution;

	AbstractMSRKinectImageStreamResolutionStrategy() : m_eOptimalResolution(NUI_IMAGE_RESOLUTION_INVALID) {}

	virtual NUI_IMAGE_RESOLUTION GetOptimalResolution()
	{
		return m_eOptimalResolution;
	}

	void setResoloutionIfLowerThanCurrent(NUI_IMAGE_RESOLUTION eResolution)
	{
		if (m_eOptimalResolution == NUI_IMAGE_RESOLUTION_INVALID || m_eOptimalResolution > eResolution) {
			m_eOptimalResolution = eResolution;
		}
	}
};

class MSRKinectColorImageStreamResolutionStrategy : public AbstractMSRKinectImageStreamResolutionStrategy
{
public:
	virtual XnStatus AddRequirement(NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution)
	{
		switch (eImageType) {
		case NUI_IMAGE_TYPE_COLOR:
			switch (eResolution) {
			case NUI_IMAGE_RESOLUTION_640x480:
			case NUI_IMAGE_RESOLUTION_1280x1024:
				setResoloutionIfLowerThanCurrent(eResolution);
				return XN_STATUS_OK;
			}
			break;
		}
		return XN_STATUS_BAD_PARAM;
	}
};

class MSRKinectDepthImageStreamResolutionStrategy : public AbstractMSRKinectImageStreamResolutionStrategy
{
public:
	virtual XnStatus AddRequirement(NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution)
	{
		switch (eImageType) {
		case NUI_IMAGE_TYPE_DEPTH:
			switch (eResolution) {
			case NUI_IMAGE_RESOLUTION_80x60:
			case NUI_IMAGE_RESOLUTION_320x240:
			case NUI_IMAGE_RESOLUTION_640x480:
				setResoloutionIfLowerThanCurrent(eResolution);
				return XN_STATUS_OK;
			}
			break;
		case NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX:
			switch (eResolution) {
			case NUI_IMAGE_RESOLUTION_80x60:
			case NUI_IMAGE_RESOLUTION_320x240:
				setResoloutionIfLowerThanCurrent(eResolution);
				return XN_STATUS_OK;
			}
			break;
		}
		return XN_STATUS_BAD_PARAM;
	}
};
