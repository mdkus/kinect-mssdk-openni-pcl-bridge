#pragma once
#include "base.h"
#include "MSRKinectStreamContextBase.h"

template <class FrameClass>
class MSRKinectStreamContext : public MSRKinectStreamContextBase
{
protected:
	const FrameClass* m_pFrame;

public:
	MSRKinectStreamContext() : m_pFrame(NULL) {}
	virtual ~MSRKinectStreamContext() {}

	HRESULT GetNextFrame()
	{
		LockFrame();
		ReleaseFrame();
		HRESULT hr = NuiImageStreamGetNextFrame(m_hStreamHandle, 100, &m_pFrame);
		if (SUCCEEDED(hr)) {
			m_nFrameID++;
			m_lTimestamp = m_pFrame->liTimeStamp.QuadPart;
		}
		UnlockFrame();
		return hr;
	}

	void ReleaseFrame()
	{
		LockFrame();
		if (m_pFrame) {
			NuiImageStreamReleaseFrame(m_hStreamHandle, m_pFrame);
			m_pFrame = NULL;
		}
		UnlockFrame();
	}

	BOOL IsFrameAvailable() {
		return m_pFrame != NULL;
	}

	const FrameClass* LockFrame()
	{
		EnterCriticalSection(&m_csFrameAccess);
		return m_pFrame;
	}

	void UnlockFrame()
	{
		LeaveCriticalSection(&m_csFrameAccess);
	}
};
