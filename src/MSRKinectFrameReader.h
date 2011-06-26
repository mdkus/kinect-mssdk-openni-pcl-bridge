#pragma once
#include "base.h"
#include "MSRKinectFrameContext.h"
#include <vector>
#include <algorithm>

template <class ContextClass>
class MSRKinectFrameReader : public ContextClass
{
private:
	typedef MSRKinectFrameReader<ContextClass> ThisClass;

public:
	class Listener {
	public:
		virtual ~Listener() {}
		virtual void OnUpdateFrame() = 0;
	};
	typedef std::vector<Listener*> ListenerList;

	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

private:
	XN_THREAD_HANDLE m_hReaderThread;
	ListenerList m_listeners;

	ChangeEvent m_generatingEvent;

public:
	ChangeEvent* GetGeneratingEvent() { return &m_generatingEvent; }

public:
	MSRKinectFrameReader() : m_hReaderThread(NULL)
	{
	}

	virtual ~MSRKinectFrameReader()
	{
		Stop();
	}

	void AddListener(Listener* listener)
	{
		if (std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) {
			m_listeners.push_back(listener);
		}
	}

	void RemoveListener(Listener* listener)
	{
		ListenerList::iterator i = std::find(m_listeners.begin(), m_listeners.end(), listener);
		if (i != m_listeners.end()) {
			m_listeners.erase(i);
		}
	}

	XnStatus Start()
	{
		if (m_bRunning) {
			return XN_STATUS_INVALID_OPERATION;
		}

		try {
			m_bRunning = TRUE;
			CHECK_XN_STATUS(xnOSCreateThread(ReaderThread, this, &m_hReaderThread));
			m_generatingEvent.Raise();
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			m_bRunning = FALSE;
			return e.nStatus;
		}
	}

	void Stop()
	{
		if (!m_bRunning) {
			return; // ignore
		}

		m_bRunning = FALSE;
		SetEvent(m_hNextFrameEvent); // awake the worker thread by sending a fake event
		xnOSWaitForThreadExit(m_hReaderThread, 100);
		m_generatingEvent.Raise();
	}

private:
	static XN_THREAD_PROC ReaderThread(void* pCookie)
	{
		ThisClass* that = (ThisClass*)pCookie;

		while (that->m_bRunning) {
			XnStatus nStatus = xnOSWaitEvent(that->m_hNextFrameEvent, INFINITE);
			if (nStatus == XN_STATUS_OK && that->m_bRunning) {
				HRESULT hr = that->GetNextFrame();
				if (SUCCEEDED(hr)) {
					for (ListenerList::iterator i = that->m_listeners.begin(); i != that->m_listeners.end(); i++) {
						(*i)->OnUpdateFrame();
					}
				}
			}
		}

		XN_THREAD_PROC_RETURN(0);
	}
};
