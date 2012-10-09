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

// FIXME: copy-and-edited from ImageGenerator. I don't like it.

#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"
#include "MSRKinectMirrorCap.h"

class MSRKinectIRGenerator :
	public AbstractMSRKinectMapGenerator<xn::ModuleIRGenerator, USHORT, XnIRPixel>,
	public virtual MSRKinectMirrorCap
{
private:
	typedef AbstractMSRKinectMapGenerator<xn::ModuleIRGenerator, USHORT, XnIRPixel> SuperClass;

private:
	static ImageConfiguration::Desc* GetImageConfigDesc()
	{
		static ImageConfiguration::Mode s_modes[] = {
			ImageConfiguration::Mode(640, 480, 30)
		};
		static ImageConfiguration::Desc s_desc(s_modes, 1);
		return &s_desc;
	}

public:
	MSRKinectIRGenerator() : SuperClass(XN_NODE_TYPE_IR, TRUE, GetImageConfigDesc())
	{
	}

	virtual ~MSRKinectIRGenerator() {}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			SuperClass::IsCapabilitySupported(strCapabilityName) ||
			MSRKinectMirrorCap::IsCapabilitySupported(strCapabilityName);
	}

	// ImageGenerator methods
	virtual XnIRPixel* GetIRMap()
	{
		return m_pBuffer;
	}

	virtual XnBool IsPixelFormatSupported(XnPixelFormat format)
	{
		return format == XN_PIXEL_FORMAT_GRAYSCALE_16_BIT;
	}

	virtual XnStatus SetPixelFormat(XnPixelFormat format) {
		return (format == XN_PIXEL_FORMAT_GRAYSCALE_16_BIT) ? XN_STATUS_OK : XN_STATUS_BAD_PARAM;
	}

	virtual XnPixelFormat GetPixelFormat()
	{
		return XN_PIXEL_FORMAT_GRAYSCALE_16_BIT;
	}

	virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; } // ignore
	virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback) {} // ignore

	MSRKinectMirrorCap_IMPL(m_pReader);

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		assert(lockedRect.Pitch == GetXRes() * sizeof(USHORT));

		const USHORT* sp = data;
		XnIRPixel* dp = m_pBuffer;

		XnUInt32 xRes = GetXRes();
		XnUInt32 yRes = GetYRes();
		int step = m_pReader->GetMirrorFactor();
		for (XnUInt y = 0; y < yRes; y++) {
			sp = data + y * xRes + (step < 0 ? xRes-1 : 0);
			for (XnUInt x = 0; x < xRes; x++) {
				*dp = *sp;
				sp += step;
				dp++;
			}
		}

		return XN_STATUS_OK;
	}

};
