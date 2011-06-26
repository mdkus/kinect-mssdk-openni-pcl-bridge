#pragma once
#include "base.h"

class MSRKinectStreamContextBase
{
public:
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

protected:
	HANDLE m_hStreamHandle;
	HANDLE m_hNextFrameEvent;

	BOOL m_bRunning;

	XnUInt32 m_nFrameID;
	XnUInt64 m_lTimestamp;

	CRITICAL_SECTION m_csFrameAccess;

	BOOL m_bMirror; // Note; flag only
	ChangeEvent m_mirrorChangeEvent;
	BOOL m_bCalibrateViewPoint; // Note: flag only
	ChangeEvent m_calibrateViewPointChangeEvent;

public:
	XnUInt32 GetFrameID() const { return m_nFrameID; }
	XnUInt64 GetTimestamp() const { return m_lTimestamp; }
	BOOL IsRunning() const { return m_bRunning; }

	BOOL IsMirror() const { return m_bMirror; }
	void SetMirror(BOOL value) { m_bMirror = value; m_mirrorChangeEvent.Raise(); }
	ChangeEvent GetMirrorChangeEvent() { return m_mirrorChangeEvent; }

	BOOL IsCalibrateViewPoint() const { return m_bCalibrateViewPoint; }
	void SetCalibrateViewPoint(BOOL value) { m_bCalibrateViewPoint = value; m_calibrateViewPointChangeEvent.Raise(); }
	ChangeEvent GetCalibrateViewPointChangeEvent() { return m_calibrateViewPointChangeEvent; }

public:
	MSRKinectStreamContextBase() :
		m_hNextFrameEvent(NULL), m_hStreamHandle(NULL),
		m_bRunning(FALSE),
		m_nFrameID(0), m_lTimestamp(0)
	{
		InitializeCriticalSection(&m_csFrameAccess);
	}

	virtual ~MSRKinectStreamContextBase()
	{
		DeleteCriticalSection(&m_csFrameAccess);
	}

	virtual void Init(HANDLE hStreamHandle, HANDLE hNextFrameEvent)
	{
		m_hStreamHandle = hStreamHandle;
		m_hNextFrameEvent = hNextFrameEvent;
	}

};
