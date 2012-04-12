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

// This UserGenerator pretends to require psi pose and raises the pose detection events
// when starting to track a new user. This is intended to be compatible with the legacy
// OpenNI applications, but may confuse them anyway because the event sequence may not be
// exactly as they expect.
class MSRKinectUserSkeletonGeneratorWithPsiPoseEmulation :
	public MSRKinectUserSkeletonGenerator,
	public xn::ModulePoseDetectionInteface
{
private:
	typedef MSRKinectUserSkeletonGenerator SuperClass;

	struct PoseDetectionCallbackHandleSet {
		XnCallbackHandle hPoseDetectionStart;
		XnCallbackHandle hPoseDetectionEnd;

		PoseDetectionCallbackHandleSet() : hPoseDetectionStart(NULL), hPoseDetectionEnd(NULL) {}
	};
	typedef std::vector<PoseDetectionCallbackHandleSet*> PoseDetectionCallbackHandleSetList;

	XN_DECLARE_EVENT_2ARG(PoseDetectionEvent, PoseDetectionEventInterface, const XnChar*, strPose, XnUserID, user);

private:
	PoseDetectionCallbackHandleSetList m_poseDetectionCallbackHandleSets;
	PoseDetectionEvent m_poseDetectionStartEvent;
	PoseDetectionEvent m_poseDetectionEndEvent;

public:

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			SuperClass::IsCapabilitySupported(strCapabilityName) ||
			strcmp(strCapabilityName, XN_CAPABILITY_POSE_DETECTION) == 0;
	}

	virtual ModulePoseDetectionInteface* GetPoseDetectionInteface() { return this; }

	virtual XnBool NeedPoseForCalibration()
	{
		return TRUE;
	}

	virtual XnStatus GetCalibrationPose(XnChar* strPose)
	{
		strcpy(strPose, psi());
		return XN_STATUS_OK;
	}

	// pose detection methods

	XnUInt32 GetNumberOfPoses(void)
	{
		return 1; // Psi pose only
	}

	XnStatus GetAvailablePoses(XnChar** pstrPoses, XnUInt32& nPoses)
	{
		return GetAllAvailablePoses(pstrPoses, strlen(psi())+1, nPoses);
	}

	XnStatus GetAllAvailablePoses(XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32& nPoses)
	{
		if (nPoses < 1) {
			return XN_STATUS_BAD_PARAM;
		}

		nPoses = 1;
		strncpy(pstrPoses[0], psi(), nNameLength);
		return XN_STATUS_OK;
	}

	XnStatus StartPoseDetection(const XnChar*, XnUserID) { return XN_STATUS_OK; } // ignore
	XnStatus StopPoseDetection(XnUserID) { return XN_STATUS_OK; } // ignore
	XnStatus StopSinglePoseDetection(XnUserID user, const XnChar* strPose) { return XN_STATUS_OK; } // ignore

	XnStatus RegisterToPoseDetectionCallbacks(XnModulePoseDetectionCallback StartPoseCB, XnModulePoseDetectionCallback EndPoseCB, void* pCookie, XnCallbackHandle& hCallback)
	{
		PoseDetectionCallbackHandleSet* pCallbackHandleSet = new PoseDetectionCallbackHandleSet();
		if (StartPoseCB) m_poseDetectionStartEvent.Register((PoseDetectionEvent::HandlerPtr)StartPoseCB, pCookie, &pCallbackHandleSet->hPoseDetectionStart);
		if (EndPoseCB) m_poseDetectionEndEvent.Register((PoseDetectionEvent::HandlerPtr)EndPoseCB, pCookie, &pCallbackHandleSet->hPoseDetectionEnd);
		m_poseDetectionCallbackHandleSets.push_back(pCallbackHandleSet);
		hCallback = pCallbackHandleSet;
		return XN_STATUS_OK;
	}

	void UnregisterFromPoseDetectionCallbacks(XnCallbackHandle hCallback)
	{
		PoseDetectionCallbackHandleSetList::iterator i = 
			std::find(m_poseDetectionCallbackHandleSets.begin(), m_poseDetectionCallbackHandleSets.end(), hCallback);
		if (i != m_poseDetectionCallbackHandleSets.end()) {
			m_poseDetectionStartEvent.Unregister((*i)->hPoseDetectionStart);
			m_poseDetectionEndEvent.Unregister((*i)->hPoseDetectionEnd);
			delete *i;
			m_poseDetectionCallbackHandleSets.erase(i);
		}
	}

	XnStatus RegisterToPoseDetectionInProgress(XnModulePoseDetectionInProgressCallback cb, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromPoseDetectionInProgress(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

	XnStatus RegisterToPoseDetected(XnModulePoseDetectionCallback cb, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromPoseDetected(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

	XnStatus RegisterToOutOfPose(XnModulePoseDetectionCallback cb, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromOutOfPose(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

protected:
	virtual void OnPreCalibration(XnUserID userID)
	{
		m_poseDetectionStartEvent.Raise(psi(), userID);
	}

	virtual void OnPostCalibration(XnUserID userID)
	{
		m_poseDetectionEndEvent.Raise(psi(), userID);
	}

private:
	const XnChar* psi() { return "Psi"; }
};
