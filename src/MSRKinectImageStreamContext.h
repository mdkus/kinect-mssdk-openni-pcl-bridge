#pragma once
#include "base.h"
#include "MSRKinectFrameContext.h"

class MSRKinectImageStreamContext : public MSRKinectFrameContext<const NUI_IMAGE_FRAME>
{
private:
	typedef MSRKinectFrameContext<const NUI_IMAGE_FRAME> SuperClass;

protected:
	HANDLE m_hStreamHandle;

protected:
	MSRKinectImageStreamContext() : m_hStreamHandle(NULL)
	{
	}

	void InitContext(HANDLE hStreamHandle, HANDLE hNextFrameEvent)
	{
		SuperClass::InitContext(hNextFrameEvent);
		m_hStreamHandle = hStreamHandle;
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
