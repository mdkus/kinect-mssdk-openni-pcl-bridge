#pragma once;
#include "base.h"
#include "MSRKinectFrameReader.h"
#include "MSRKinectSkeletonContext.h"

class MSRKinectSkeletonReader : public MSRKinectFrameReader<MSRKinectSkeletonContext>
{
public:
	void Init(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent)
	{
		InitContext(pRequirement, hNextFrameEvent);
		m_pRequirement->AddCapabilityRequirement(XN_CAPABILITY_SKELETON);
	}

	virtual void Setup()
	{
		m_pRequirement->DoInitialize();

		CHECK_HRESULT(NuiSkeletonTrackingEnable(m_hNextFrameEvent, 0));
	}
};
