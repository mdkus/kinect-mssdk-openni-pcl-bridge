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
#include "MSRKinectAudioStreamReader.h"
#include "MSRKinectRequirement.h"

class MSRKinectAudioStreamManager
{
protected:
	HANDLE m_hNextFrameEvent;
	MSRKinectAudioStreamReader* m_pReader;

public:
	MSRKinectAudioStreamReader* GetReader() { return m_pReader; }

public:
	MSRKinectAudioStreamManager(MSRKinectRequirement* pRequirement) :
	  m_hNextFrameEvent(NULL), m_pReader(NULL)
		// throws XnStatusException
	{
		CHECK_XN_STATUS(xnOSCreateEvent(&m_hNextFrameEvent, TRUE));
		m_pReader = new MSRKinectAudioStreamReader(pRequirement, m_hNextFrameEvent);
	}

	virtual ~MSRKinectAudioStreamManager()
	{
		if (m_pReader) delete m_pReader;
		if (m_hNextFrameEvent) xnOSCloseEvent(&m_hNextFrameEvent);
	}

	void AddListener(MSRKinectAudioStreamReader::IListener* listener)
	{
		m_pReader->AddListener(listener);
	}

	void RemoveListener(MSRKinectAudioStreamReader::IListener* listener)
	{
		m_pReader->RemoveListener(listener);
	}
};
