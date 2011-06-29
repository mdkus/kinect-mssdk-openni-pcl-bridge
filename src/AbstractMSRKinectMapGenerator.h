#pragma once
#include "base.h"
#include "AbstractMSRKinectImageStreamGenerator.h"

template <class ParentMapGeneratorClass, class SourcePixelType, class TargetPixelType, class ImageConfigurationClass>
class AbstractMSRKinectMapGenerator : public virtual AbstractMSRKinectImageStreamGenerator<ParentMapGeneratorClass, SourcePixelType, TargetPixelType, ImageConfigurationClass>
{
protected:
	AbstractMSRKinectMapGenerator() {}

public:
	// MapGenerator methods

	virtual XnUInt32 GetSupportedMapOutputModesCount()
	{
		return 1; // 640x480x30fps only
	}

	virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
	{
		for (XnUInt32 i = 0; i < nCount && i < m_imageConfig.GetNumberOfSupportedModes(); i++) {
			aModes[i] = m_imageConfig.GetSupportedModeAt(i)->outputMode;
		}
		return XN_STATUS_OK;
	}

	virtual XnStatus SetMapOutputMode(const XnMapOutputMode& mode)
	{
		try {
			CHECK_XN_STATUS(m_imageConfig.Select(mode));
			m_pReader->SetResolution(m_imageConfig.GetSelectedMode()->eResolution);
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	virtual XnStatus GetMapOutputMode(XnMapOutputMode& mode)
	{
		mode = m_imageConfig.GetSelectedMode()->outputMode;
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		// ignore, maybe someday
		return XN_STATUS_OK;
	}

	virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
	{
		// ignore, maybe someday
	}
};
