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
#include "MSRKinectUserSkeletonGenerator.h"

class MSRKinectUserSkeletonGeneratorWithAutoElevation :
	public MSRKinectUserSkeletonGenerator
{
private:
	typedef MSRKinectUserSkeletonGenerator SuperClass;

private:
	LONGLONG m_liTimestamp;

public:
	MSRKinectUserSkeletonGeneratorWithAutoElevation() : m_liTimestamp(0)
	{
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		XnStatus result = SuperClass::UpdateImageData(pFrame, data, lockedRect);
		if (result != XN_STATUS_OK) return result;

		changeElevation();

		return XN_STATUS_OK;
	}

private:
	void changeElevation()
	{
		if (abs(m_skeletonFrame.liTimeStamp.QuadPart - m_liTimestamp) > 5000) {
			XnUserID userIDs[6];
			XnUInt16 users = 6;
			GetUsers(userIDs, users);
			if (users > 0) {
				NUI_SKELETON_DATA* sd = GetSkeletonData(userIDs[0]);
				DWORD clippedTop = sd->dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_TOP;
				DWORD clippedBottom = sd->dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_BOTTOM;
				LONG angle;
				NuiCameraElevationGetAngle(&angle);
				if (clippedTop && !clippedBottom) {
					angle += 5;
				} else if (clippedBottom && !clippedTop) {
					angle -= 5;
				}
				NuiCameraElevationSetAngle(angle);
			}
			m_liTimestamp = m_skeletonFrame.liTimeStamp.QuadPart;
		}
	}

};
