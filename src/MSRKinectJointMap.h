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

class MSRKinectJointMap
{
private:
	static const int XN_SKEL_MAX = 25;
	static MSRKinectJointMap cs_instance;

	XnSkeletonJoint m_nuiIndexToXnJoint[NUI_SKELETON_POSITION_COUNT];
	NUI_SKELETON_POSITION_INDEX m_xnJointToNuiIndex[XN_SKEL_MAX];

private:
	MSRKinectJointMap() {
		xnOSMemSet(m_nuiIndexToXnJoint, 0, sizeof(m_nuiIndexToXnJoint));
		xnOSMemSet(m_xnJointToNuiIndex, 0xff, sizeof(m_xnJointToNuiIndex));

		addPair(XN_SKEL_WAIST, NUI_SKELETON_POSITION_HIP_CENTER);
		addPair(XN_SKEL_TORSO, NUI_SKELETON_POSITION_SPINE);
		addPair(XN_SKEL_NECK, NUI_SKELETON_POSITION_SHOULDER_CENTER);
		addPair(XN_SKEL_HEAD, NUI_SKELETON_POSITION_HEAD);
		addPair(XN_SKEL_LEFT_SHOULDER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
		addPair(XN_SKEL_LEFT_ELBOW, NUI_SKELETON_POSITION_ELBOW_LEFT);
		addPair(XN_SKEL_LEFT_WRIST, NUI_SKELETON_POSITION_WRIST_LEFT);
		addPair(XN_SKEL_LEFT_HAND, NUI_SKELETON_POSITION_HAND_LEFT);
		addPair(XN_SKEL_RIGHT_SHOULDER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);
		addPair(XN_SKEL_RIGHT_ELBOW, NUI_SKELETON_POSITION_ELBOW_RIGHT);
		addPair(XN_SKEL_RIGHT_WRIST, NUI_SKELETON_POSITION_WRIST_RIGHT);
		addPair(XN_SKEL_RIGHT_HAND, NUI_SKELETON_POSITION_HAND_RIGHT);
		addPair(XN_SKEL_LEFT_HIP, NUI_SKELETON_POSITION_HIP_LEFT);
		addPair(XN_SKEL_LEFT_KNEE, NUI_SKELETON_POSITION_KNEE_LEFT);
		addPair(XN_SKEL_LEFT_ANKLE, NUI_SKELETON_POSITION_ANKLE_LEFT);
		addPair(XN_SKEL_LEFT_FOOT, NUI_SKELETON_POSITION_FOOT_LEFT);
		addPair(XN_SKEL_RIGHT_HIP, NUI_SKELETON_POSITION_HIP_RIGHT);
		addPair(XN_SKEL_RIGHT_KNEE, NUI_SKELETON_POSITION_KNEE_RIGHT);
		addPair(XN_SKEL_RIGHT_ANKLE, NUI_SKELETON_POSITION_ANKLE_RIGHT);
		addPair(XN_SKEL_RIGHT_FOOT, NUI_SKELETON_POSITION_FOOT_RIGHT);
	};

	void addPair(XnSkeletonJoint xnJoint, NUI_SKELETON_POSITION_INDEX nuiIndex)
	{
		m_nuiIndexToXnJoint[nuiIndex] = xnJoint;
		m_xnJointToNuiIndex[xnJoint] = nuiIndex;
	}

public:
	static XnSkeletonJoint GetXnJointByNuiIndex(NUI_SKELETON_POSITION_INDEX nuiIndex) {
		return cs_instance.m_nuiIndexToXnJoint[nuiIndex];
	}

	static NUI_SKELETON_POSITION_INDEX GetNuiIndexByXnJoint(XnSkeletonJoint xnJoint) {
		return cs_instance.m_xnJointToNuiIndex[xnJoint];
	}

};
