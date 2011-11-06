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
