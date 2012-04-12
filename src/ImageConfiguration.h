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

class ImageConfiguration
{
public:
	class Mode
	{
	public:
		XnMapOutputMode outputMode;

		Mode(XnUInt32 nXRes, XnUInt32 nYRes, XnUInt32 nFPS)
		{
			outputMode.nXRes = nXRes;
			outputMode.nYRes = nYRes;
			outputMode.nFPS = nFPS;
		}
	};

	class Desc
	{
	public:
		const Mode* supportedModes;
		XnUInt32 numberOfSupportedModes;

		Desc(Mode* _supportedModes, XnUInt32 _numberOfSupportedModes) :
			supportedModes(_supportedModes), numberOfSupportedModes(_numberOfSupportedModes) {}
	};

private:
	const Desc* m_pDesc;
	XnUInt32 m_selectedModeIndex;

public:
	const Mode* GetSupportedModes() const { return m_pDesc->supportedModes; }
	XnUInt32 GetNumberOfSupportedModes() const { return m_pDesc->numberOfSupportedModes; }

	XnUInt32 GetSelectedModeIndex() const { return m_selectedModeIndex; }
	void SetSelectedModeIndex(XnUInt32 index) { m_selectedModeIndex = index; }
	const Mode* GetSelectedMode() const { return GetSupportedModeAt(m_selectedModeIndex); }
	const Mode* GetSupportedModeAt(XnUInt32 index) const { return GetSupportedModes() + index; }

public:
	ImageConfiguration(const Desc* pDesc) : m_pDesc(pDesc), m_selectedModeIndex(0) {}

	XnStatus Select(const XnMapOutputMode& mode)
	{
		for (XnUInt32 i = 0; i < GetNumberOfSupportedModes(); i++) {
			// check resolution. ignore FPS.
			if (GetSupportedModeAt(i)->outputMode.nXRes == mode.nXRes &&
					GetSupportedModeAt(i)->outputMode.nYRes == mode.nYRes) {
				SetSelectedModeIndex(i);
				return XN_STATUS_OK;
			}
		}
		return XN_STATUS_BAD_PARAM;
	}
};
