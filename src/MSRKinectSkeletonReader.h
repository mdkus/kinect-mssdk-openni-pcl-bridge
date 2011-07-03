#pragma once;
#include "base.h"
#include "MSRKinectFrameReader.h"
#include "MSRKinectSkeletonContext.h"

class MSRKinectSkeletonReader :
	public MSRKinectFrameReader<MSRKinectSkeletonContext>
{
private:
	 typedef MSRKinectFrameReader<MSRKinectSkeletonContext> SuperClass;

public:
	MSRKinectSkeletonReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) : SuperClass(pRequirement, hNextFrameEvent)
	{
		m_pRequirement->AddCapabilityRequirement(XN_CAPABILITY_SKELETON);
	}

	virtual void Setup()
	{
		m_pRequirement->DoInitialize();

		CHECK_HRESULT(NuiSkeletonTrackingEnable(m_hNextFrameEvent, 0));
	}
};
