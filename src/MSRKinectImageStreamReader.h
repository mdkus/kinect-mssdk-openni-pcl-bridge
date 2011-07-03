#pragma once
#include "MSRKinectFrameReader.h"
#include "MSRKinectImageStreamContext.h"
#include "ImageConfiguration.h"
#include "MSRKinectRequirement.h"

class MSRKinectImageStreamReader : public MSRKinectFrameReader<MSRKinectImageStreamContext>
{
public:
	MSRKinectImageStreamReader()
	{
	}

	void SetOutputMode(XnPredefinedProductionNodeType nodeType, const XnMapOutputMode& mode)
	{
		m_pRequirement->AddRequirement(nodeType, mode);
	}

	void Init(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) {
		InitContext(pRequirement, hNextFrameEvent);
	}

protected:
	virtual void Setup()
	{
		if (!m_hStreamHandle) {
			m_pRequirement->DoInitialize();
			SetupStreamInfo();
			CHECK_HRESULT(NuiImageStreamOpen(m_eImageType, m_eImageResolution, 0, 2, m_hNextFrameEvent, &m_hStreamHandle));
		}
	}

	virtual void SetupStreamInfo() = 0;

};

class MSRKinectColorImageStreamReader : public MSRKinectImageStreamReader
{
	virtual void SetupStreamInfo()
	{
		m_eImageType = NUI_IMAGE_TYPE_COLOR;
		m_eImageResolution = m_pRequirement->GetColorImageResolution();
	}
};

class MSRKinectDepthImageStreamReader : public MSRKinectImageStreamReader
{
	virtual void SetupStreamInfo()
	{
		m_eImageType = m_pRequirement->GetDepthImageType();
		m_eImageResolution = m_pRequirement->GetDepthImageResolution();
	}
};
