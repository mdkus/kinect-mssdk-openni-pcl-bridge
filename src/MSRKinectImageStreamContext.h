#pragma once
#include "base.h"
#include "MSRKinectFrameContext.h"

class MSRKinectImageStreamContext : public MSRKinectFrameContext<const NUI_IMAGE_FRAME>
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
		// Weird... Only NUI_IMAGE_TYPE_DEPTH comes with flipped data
		return m_bMirror ^ (m_eImageType == NUI_IMAGE_TYPE_DEPTH) ? 1 : -1;
	}

protected:
	MSRKinectImageStreamContext() : m_hStreamHandle(NULL)
	{
	}

	void InitContext(HANDLE hNextFrameEvent, NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eImageResolution)
	{
		SuperClass::InitContext(hNextFrameEvent);
		m_eImageType = eImageType;
		m_eImageResolution = eImageResolution;
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
