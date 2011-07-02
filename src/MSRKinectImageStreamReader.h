#pragma once
#include "MSRKinectFrameReader.h"
#include "MSRKinectImageStreamContext.h"
#include "ImageConfiguration.h"
#include "MSRKinectImageStreamResolutionStrategy.h"

class MSRKinectImageStreamReader : public MSRKinectFrameReader<MSRKinectImageStreamContext>
{
protected:
	MSRKinectImageStreamResolutionStrategy* m_pResolutionStrategy;

public:
	MSRKinectImageStreamReader(MSRKinectImageStreamResolutionStrategy* pResolutionStrategy) :
		m_pResolutionStrategy(pResolutionStrategy)
	{
	}

	void SetResolution(NUI_IMAGE_RESOLUTION eResolution)
	{
		if (m_hStreamHandle) {
			// you can change the resolution only before start
			throw new XnStatusException(XN_STATUS_INVALID_OPERATION);
		}

		m_pResolutionStrategy->AddRequirement(m_eImageType, eResolution);
		m_eImageResolution = m_pResolutionStrategy->GetOptimalResolution();
	}

public:
	void Init(HANDLE hNextFrameEvent, NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eImageResolution) {
		InitContext(hNextFrameEvent, eImageType, NUI_IMAGE_RESOLUTION_INVALID);
		SetResolution(eImageResolution);
	}

	virtual void Setup()
	{
		if (!m_hStreamHandle) {
			CHECK_HRESULT(NuiImageStreamOpen(m_eImageType, m_eImageResolution, 0, 2, m_hNextFrameEvent, &m_hStreamHandle));
		}
	}
};
