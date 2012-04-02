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
			PostStreamOpen();
		}
	}

	virtual void SetupStreamInfo() = 0;
	virtual void PostStreamOpen() {}

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

	virtual void PostStreamOpen()
	{
		// Set flag on depth stream to set far points to non-zero.
		// -> I decided to take it out eventually because it did not work well with NITE's user detector.

		// DWORD depthImageStreamFlags;
		// CHECK_HRESULT(NuiImageStreamGetImageFrameFlags(m_hStreamHandle, &depthImageStreamFlags));
		// CHECK_HRESULT(NuiImageStreamSetImageFrameFlags(m_hStreamHandle, depthImageStreamFlags | NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES));
	}
};
