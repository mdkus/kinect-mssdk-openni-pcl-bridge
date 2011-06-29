#pragma once
#include "MSRKinectFrameReader.h"
#include "MSRKinectImageStreamContext.h"
#include "ImageConfiguration.h"

class MSRKinectImageStreamReader : public MSRKinectFrameReader<MSRKinectImageStreamContext>
{
public:
	void SetResolution(NUI_IMAGE_RESOLUTION eResolution)
	{
		if (m_hStreamHandle) {
			// you can change the resolution only before start
			throw new XnStatusException(XN_STATUS_INVALID_OPERATION);
		}

		m_eImageResolution = eResolution;
	}

public:
	void Init(HANDLE hNextFrameEvent, NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eImageResolution) {
		InitContext(hNextFrameEvent, eImageType, eImageResolution);
	}

	virtual void Setup()
	{
		if (!m_hStreamHandle) {
			CHECK_HRESULT(NuiImageStreamOpen(m_eImageType, m_eImageResolution, 0, 2, m_hNextFrameEvent, &m_hStreamHandle));
		}
	}
};
