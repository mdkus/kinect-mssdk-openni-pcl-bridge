#pragma once
#include "base.h"
#include "AbstractMSRKinectImageStreamGenerator.h"

template <class ParentMapGeneratorClass, class SourcePixelType, class TargetPixelType>
class AbstractMSRKinectMapGenerator :
	public AbstractMSRKinectImageStreamGenerator<ParentMapGeneratorClass, SourcePixelType, TargetPixelType>
{
protected:
	AbstractMSRKinectMapGenerator(XnPredefinedProductionNodeType nodeType, BOOL bActiveGeneratorControl, const ImageConfiguration::Desc* pImageConfigDesc) :
		 AbstractMSRKinectImageStreamGenerator(nodeType, bActiveGeneratorControl, pImageConfigDesc)
	{
	}

public:
	// MapGenerator methods

	virtual XnUInt32 GetSupportedMapOutputModesCount()
	{
		return m_imageConfig.GetNumberOfSupportedModes();
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
			m_pReader->SetOutputMode(m_nodeType, m_imageConfig.GetSelectedMode()->outputMode);
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

protected:
	void PopulateMapMetaData(XnMapMetaData* pMetaData)
	{
		XnMapOutputMode mode;
		GetMapOutputMode(mode);

		// assume we don't support cropping
		pMetaData->FullRes.X = pMetaData->Res.X = mode.nXRes;
		pMetaData->FullRes.Y = pMetaData->Res.Y = mode.nYRes;
		pMetaData->nFPS = mode.nFPS;
		pMetaData->Offset.X = pMetaData->Offset.Y = 0;

		// get the data for frame
		XnUInt64 dummyTimestamp;
		pMetaData->pOutput->bIsNew = IsNewDataAvailable(dummyTimestamp); // I don't know if this is OK
		pMetaData->pOutput->nDataSize = GetDataSize();
		pMetaData->pOutput->nFrameID = GetFrameID();
		pMetaData->pOutput->nTimestamp = GetTimestamp();
	}
};
