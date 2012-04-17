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

#include "base.h"
#include "MSRKinectFrameContextBase.h"
#include "DmoAudioReader.h"

class MSRKinectAudioStreamContext : public MSRKinectFrameContextBase
{
private:
	typedef MSRKinectFrameContextBase SuperClass;

protected:
	DmoAudioReader* m_pDmoAudio;

public:
	MSRKinectAudioStreamContext(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		SuperClass(pRequirement, hNextFrameEvent), m_pDmoAudio(NULL)
	{
	}

	HRESULT GetNextFrame()
	{
		// FIXME double buffering
		HRESULT hr = m_pDmoAudio->Read();
		if (hr == S_OK) {
			m_nFrameID++;
			m_lTimestamp = m_pDmoAudio->GetTimestamp();
		}
		return hr;
	}

	BYTE* GetData() { return m_pDmoAudio ? m_pDmoAudio->GetData() : NULL; }
	DWORD GetDataSize() { return m_pDmoAudio ? m_pDmoAudio->GetDataSize() : 0; }
	double GetBeamAngle() { return m_pDmoAudio ? m_pDmoAudio->GetBeamAngle() : 0; }
	double GetSourceAngle() { return m_pDmoAudio ? m_pDmoAudio->GetSourceAngle() : 0; }
	double GetSourceAngleConfidence() { return m_pDmoAudio ? m_pDmoAudio->GetSourceAngleConfidence() : 0; }

protected:
	void SetUpDmoAudio() {
		m_pDmoAudio = new DmoAudioReader(m_pRequirement->GetSensor());
	}
};

