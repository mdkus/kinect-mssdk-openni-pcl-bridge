#pragma once
#include "MSRKinectFrameReader.h"
#include "MSRKinectImageStreamContext.h"
#include "ImageConfiguration.h"
#include "MSRKinectRequirement.h"

class MSRKinectImageStreamReader :
	public MSRKinectFrameReader<MSRKinectImageStreamContext>
{
private:
	typedef MSRKinectFrameReader<MSRKinectImageStreamContext> SuperClass;

public:
	MSRKinectImageStreamReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		SuperClass(pRequirement, hNextFrameEvent)
	{
	}

	void SetOutputMode(XnPredefinedProductionNodeType nodeType, const XnMapOutputMode& mode)
	{
		m_pRequirement->AddRequirement(nodeType, mode);
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
public:
	MSRKinectColorImageStreamReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		MSRKinectImageStreamReader(pRequirement, hNextFrameEvent)
	{
	}

protected:
	virtual void SetupStreamInfo()
	{
		m_eImageType = NUI_IMAGE_TYPE_COLOR;
		m_eImageResolution = m_pRequirement->GetColorImageResolution();
	}
};

class MSRKinectDepthImageStreamReader : public MSRKinectImageStreamReader
{
public:
	MSRKinectDepthImageStreamReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		MSRKinectImageStreamReader(pRequirement, hNextFrameEvent)
	{
	}

protected:
	virtual void SetupStreamInfo()
	{
		m_eImageType = m_pRequirement->GetDepthImageType();
		m_eImageResolution = m_pRequirement->GetDepthImageResolution();
	}
};
