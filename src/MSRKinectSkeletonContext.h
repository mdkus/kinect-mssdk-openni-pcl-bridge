#pragma once;
#include "base.h"
#include "MSRKinectFrameContext.h"

class MSRKinectSkeletonContext : public MSRKinectFrameContext<NUI_SKELETON_FRAME>
{
private:
	BOOL m_bSmooth;
	NUI_SKELETON_FRAME m_frame;

public:
	BOOL IsSmooth() const { return m_bSmooth; }
	void SetSmooth(BOOL value) { m_bSmooth = value; }

public:
	MSRKinectSkeletonContext() : m_bSmooth(FALSE) {}

	virtual HRESULT GetNextFrameImpl()
	{
		try {
			CHECK_HRESULT(NuiSkeletonGetNextFrame(0, &m_frame));
			if (m_bSmooth) {
				CHECK_HRESULT(NuiTransformSmooth(&m_frame, NULL));
			}
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
