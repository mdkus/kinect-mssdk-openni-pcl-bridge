#pragma once;
#include "base.h"
#include "MSRKinectFrameContext.h"

class MSRKinectSkeletonContext : public MSRKinectFrameContext<NUI_SKELETON_FRAME>
{
private:
	NUI_SKELETON_FRAME m_frame;

public:
	virtual HRESULT GetNextFrameImpl()
	{
		try {
			CHECK_HRESULT(NuiSkeletonGetNextFrame(0, &m_frame));
			CHECK_HRESULT(NuiTransformSmooth(&m_frame, NULL));
			m_pFrame = &m_frame;
			return S_OK;
		} catch (XnStatusException& e) {
			return e.hResult;
		}
	}

	virtual void ReleaseFrameImpl()
	{
		// nothing to do
	}

};
