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
#include "MSRKinectFrameContextBase.h"

class MSRKinectGeometricFrameContextBase : public MSRKinectFrameContextBase
{
private:
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

protected:
	BOOL m_bMirror; // Note; flag only
	ChangeEvent m_mirrorChangeEvent;
	BOOL m_bCalibrateViewPoint; // Note: flag only
	ChangeEvent m_calibrateViewPointChangeEvent;

public:
	BOOL IsMirror() const { return m_bMirror; }
	void SetMirror(BOOL value) { m_bMirror = value; m_mirrorChangeEvent.Raise(); }
	ChangeEvent* GetMirrorChangeEvent() { return &m_mirrorChangeEvent; }

	BOOL IsCalibrateViewPoint() const { return m_bCalibrateViewPoint; }
	void SetCalibrateViewPoint(BOOL value) { m_bCalibrateViewPoint = value; m_calibrateViewPointChangeEvent.Raise(); }
	ChangeEvent* GetCalibrateViewPointChangeEvent() { return &m_calibrateViewPointChangeEvent; }

protected:
	MSRKinectGeometricFrameContextBase(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		MSRKinectFrameContextBase(pRequirement, hNextFrameEvent), 
		m_bMirror(FALSE),
		m_bCalibrateViewPoint(FALSE)
	{
	}

public:
	virtual ~MSRKinectGeometricFrameContextBase() {}
};
