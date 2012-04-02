#pragma once
#include "base.h"
#include "MSRKinectFrameContext.h"

class MSRKinectImageStreamContext :
	public MSRKinectFrameContext<const NUI_IMAGE_FRAME>
{
private:
	typedef MSRKinectFrameContext<const NUI_IMAGE_FRAME> SuperClass;

protected:
	HANDLE m_hStreamHandle;
	NUI_IMAGE_TYPE m_eImageType;
	NUI_IMAGE_RESOLUTION m_eImageResolution;

public:
	NUI_IMAGE_RESOLUTION GetImageResolution() const { return m_eImageResolution; }
	NUI_IMAGE_TYPE GetImageType() const { return m_eImageType; }

	int GetMirrorFactor() const
	{
#if KINECTSDK_VER >= 100
		return m_bMirror;
#else
		// Weird... Only NUI_IMAGE_TYPE_DEPTH comes with flipped data
		return m_bMirror ^ (m_eImageType == NUI_IMAGE_TYPE_DEPTH) ? 1 : -1;
#endif
	}

protected:
	MSRKinectImageStreamContext(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		SuperClass(pRequirement, hNextFrameEvent),
		m_hStreamHandle(NULL),
		m_eImageResolution(NUI_IMAGE_RESOLUTION_INVALID)
	{
	}

	virtual HRESULT GetNextFrameImpl()
	{
		return NuiImageStreamGetNextFrame(m_hStreamHandle, 100, &m_pFrame);
	}

	virtual void ReleaseFrameImpl()
	{
		NuiImageStreamReleaseFrame(m_hStreamHandle, m_pFrame);
	}

};
