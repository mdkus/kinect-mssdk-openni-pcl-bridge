#pragma once
#include "base.h"
#include "MSRKinectFrameContextBase.h"

template <class FrameClass>
class MSRKinectFrameContext :
	public MSRKinectFrameContextBase
{
private:
	typedef MSRKinectFrameContextBase SuperClass;

protected:
	FrameClass* m_pFrame;
	CRITICAL_SECTION m_csFrameAccess;

public:
	MSRKinectFrameContext(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) : SuperClass(pRequirement, hNextFrameEvent), m_pFrame(NULL)
	{
		InitializeCriticalSection(&m_csFrameAccess);
	}
	
	virtual ~MSRKinectFrameContext()
	{
		DeleteCriticalSection(&m_csFrameAccess);
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

	HRESULT GetNextFrame()
	{
		LockFrame();
		ReleaseFrame();
		HRESULT hr = GetNextFrameImpl();
		if (SUCCEEDED(hr)) {
			m_nFrameID++;
			m_lTimestamp = m_pFrame->liTimeStamp.QuadPart * 1000;
		}
		UnlockFrame();
		return hr;
	}

	void ReleaseFrame()
	{
		LockFrame();
		if (m_pFrame) {
			ReleaseFrameImpl();
			m_pFrame = NULL;
		}
		UnlockFrame();
	}

protected:
	virtual HRESULT GetNextFrameImpl() = 0;
	virtual void ReleaseFrameImpl() = 0;
};
