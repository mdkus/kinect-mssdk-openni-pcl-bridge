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

class MSRKinectImageStreamContext :
	public MSRKinectFrameContext<NUI_IMAGE_FRAME>
{
private:
	typedef MSRKinectFrameContext<NUI_IMAGE_FRAME> SuperClass;

protected:
	HANDLE m_hStreamHandle;
	NUI_IMAGE_TYPE m_eImageType;
	NUI_IMAGE_RESOLUTION m_eImageResolution;

public:
	NUI_IMAGE_RESOLUTION GetImageResolution() const { return m_eImageResolution; }
	NUI_IMAGE_TYPE GetImageType() const { return m_eImageType; }

	int GetMirrorFactor() const
	{
#if KINECTSDK_VER >= 100
		return m_bMirror ? 1 : -1;
#else
		// Weird... Only NUI_IMAGE_TYPE_DEPTH comes with flipped data
		return m_bMirror ^ (m_eImageType == NUI_IMAGE_TYPE_DEPTH) ? 1 : -1;
#endif
	}

protected:
	MSRKinectImageStreamContext(MSRKinectRequirement* pRequirement, HANDLE hNextFrameEvent) :
		SuperClass(pRequirement, hNextFrameEvent),
		m_hStreamHandle(NULL),
		m_eImageResolution(NUI_IMAGE_RESOLUTION_INVALID)
	{
	}

	virtual HRESULT GetNextFrameImpl()
	{
		return m_pRequirement->GetSensor()->NuiImageStreamGetNextFrame(m_hStreamHandle, 100, &m_frame);
	}

	virtual void ReleaseFrameImpl()
	{
		m_pRequirement->GetSensor()->NuiImageStreamReleaseFrame(m_hStreamHandle, &m_frame);
	}

};
