//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#pragma once
#include "base.h"
#include "MSRKinectFrameContext.h"
#include "IMultiThreadFrameReader.h"
#include <vector>
#include <algorithm>

template <class ContextClass>
class MSRKinectFrameReader :
	public ContextClass,
	public virtual IMultiThreadFrameReader
{
private:
	typedef MSRKinectFrameReader<ContextClass> ThisClass;

public:
	typedef std::vector<IListener*> ListenerList;
	typedef void (IListener::*ListenerHandlerNoArg)();

private:
	XN_THREAD_HANDLE m_hReaderThread;
	ListenerList m_listeners;
	DWORD m_timeout;

public:
	MSRKinectFrameReader(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent, DWORD timeout) :
		ContextClass(pRequirement, hNextFrameEvent), m_hReaderThread(NULL), m_timeout(timeout)
	{
	}

	virtual ~MSRKinectFrameReader()
	{
		if (m_bRunning) {
			StopImpl();
		}
	}

	void AddListener(IListener* listener)
	{
		if (std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) {
			m_listeners.push_back(listener);
		}
	}

	void RemoveListener(IListener* listener)
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
			Setup();

			m_bRunning = TRUE;
			CHECK_XN_STATUS(xnOSCreateThread(ReaderThread, this, &m_hReaderThread));
			RaiseEventNoArg(&IListener::OnStartReadingFrame);
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			m_bRunning = FALSE;
			return e.nStatus;
		}
	}

	void Stop()
	{
		if (m_bRunning) {
			StopImpl();
			RaiseEventNoArg(&IListener::OnStopReadingFrame);
		}
	}

protected:
	virtual void Setup() {};

private:
	void RaiseEventNoArg(ListenerHandlerNoArg handler)
	{
		for (ListenerList::iterator i = m_listeners.begin(); i != m_listeners.end(); i++) {
			((*i)->*handler)();
		}
	}

	void StopImpl()
	{
		m_bRunning = FALSE;
		SetEvent(m_hNextFrameEvent); // awake the worker thread by sending a fake event
		xnOSWaitForThreadExit(m_hReaderThread, 100);
	}

	static XN_THREAD_PROC ReaderThread(void* pCookie)
	{
		ThisClass* that = (ThisClass*)pCookie;

		try {
			while (that->m_bRunning) {
				XnStatus nStatus = xnOSWaitEvent(that->m_hNextFrameEvent, that->m_timeout);
				if ((nStatus == XN_STATUS_OK || nStatus == XN_STATUS_OS_EVENT_TIMEOUT) && that->m_bRunning) {
					CHECK_HRESULT(that->GetNextFrame());
					that->RaiseEventNoArg(&IListener::OnUpdateFrame);
				}
			}
		} catch (XnStatusException&) {
			that->Stop();
		}

		XN_THREAD_PROC_RETURN(0);
	}
};
