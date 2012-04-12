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
#include "MSRKinectUserGenerator.h"
#include "MSRKinectSkeletonReader.h"
#include "MSRKinectJointMap.h"
#include "JointOrientationCalculator.h"

class MSRKinectUserSkeletonGenerator :
	public MSRKinectUserGenerator,
	public virtual xn::ModuleSkeletonInterface
{
private:
	typedef MSRKinectUserGenerator SuperClass;

	struct CalibrationCallbackHandleSet {
		XnCallbackHandle hCalibrationStart;
		XnCallbackHandle hCalibrationEnd;

		CalibrationCallbackHandleSet() : hCalibrationStart(NULL), hCalibrationEnd(NULL) {}
	};
	typedef std::vector<CalibrationCallbackHandleSet*> CalibrationCallbackHandleSetList;

	XN_DECLARE_EVENT_1ARG(CalibrationStartEvent, CalibrationEventStartInterface, XnUserID, user);
	XN_DECLARE_EVENT_2ARG(CalibrationEndEvent, CalibrationEventEndInterface, XnUserID, user, XnBool, isSuccess);

protected:
	MSRKinectSkeletonReader* m_pSkeletonReader;
	NUI_SKELETON_FRAME m_skeletonFrame;

	NUI_SKELETON_DATA* GetSkeletonData(XnUserID user) { return &m_skeletonFrame.SkeletonData[user-1]; };

	CalibrationCallbackHandleSetList m_calibrationCallbackHandleSets;
	CalibrationStartEvent m_calibrationStartEvent;
	CalibrationEndEvent m_calibrationEndEvent;

public:
	MSRKinectUserSkeletonGenerator() // throws XnStatusException
	{
		MSRKinectManager* pMan = MSRKinectManager::GetInstance();
		m_pSkeletonReader = pMan->GetSkeletonManager()->GetReader();
	}

	virtual XnBool IsGenerating()
	{
		return SuperClass::IsGenerating() && m_pSkeletonReader->IsRunning();
	}

	virtual XnStatus StartGenerating()
	{
		try {
			CHECK_XN_STATUS(SuperClass::StartGenerating());
			return m_pSkeletonReader->Start();
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	virtual void StopGenerating()
	{
		m_pSkeletonReader->Stop();
		SuperClass::StopGenerating();
	}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			SuperClass::IsCapabilitySupported(strCapabilityName) ||
			strcmp(strCapabilityName, XN_CAPABILITY_SKELETON) == 0;
	}

	virtual ModuleSkeletonInterface* GetSkeletonInterface() { return this; }

	virtual XnStatus GetCoM(XnUserID user, XnPoint3D& com) {
		if (IsTracking(user)) {
			copyPointToXn(&GetSkeletonData(user)->Position, &com);
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_OK; // FIXME
		}
	}

	// Skeleton methods
	virtual XnBool IsJointAvailable(XnSkeletonJoint eJoint)
	{
		return MSRKinectJointMap::GetNuiIndexByXnJoint(eJoint) >= 0;
	}

	virtual XnBool IsProfileAvailable(XnSkeletonProfile eProfile) { return TRUE; } // probably work
	virtual XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile) { return XN_STATUS_OK; } // ignore
	virtual XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState) { return XN_STATUS_OK; } // ignore
	virtual XnBool IsJointActive(XnSkeletonJoint eJoint) { return IsJointAvailable(eJoint); }

	virtual XnStatus RegisterToJointConfigurationChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; } // ignore
	virtual void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback) {} // ignore

	virtual XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints) { return XN_STATUS_OK; } // ignore

	virtual XnStatus GetSkeletonJoint(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation& jointTransformation)
	{
		GetSkeletonJointOrientation(user, eJoint, jointTransformation.orientation);
		GetSkeletonJointPosition(user, eJoint, jointTransformation.position);
		return XN_STATUS_OK;
	}

	virtual XnStatus GetSkeletonJointPosition(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition& jointPosition)
	{
		jointPosition.fConfidence = 0;
		jointPosition.position.X = 0;
		jointPosition.position.Y = 0;
		jointPosition.position.Z = 0;

		if (!IsTracking(user)) {
			return XN_STATUS_OK;
		}

		NUI_SKELETON_POSITION_INDEX index = MSRKinectJointMap::GetNuiIndexByXnJoint(eJoint);
		if (index < 0) {
			return XN_STATUS_OK;
		}

		NUI_SKELETON_POSITION_TRACKING_STATE spts = GetSkeletonData(user)->eSkeletonPositionTrackingState[index];
		
		jointPosition.fConfidence =
			(spts == NUI_SKELETON_POSITION_TRACKED) ? 1.0f :
			(spts == NUI_SKELETON_POSITION_INFERRED) ? 0.5f : 0.0f;

		copyPointToXn(&GetSkeletonData(user)->SkeletonPositions[index], &jointPosition.position);

		return XN_STATUS_OK;
	}

	virtual XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& jointOrientation)
	{
		jointOrientation.fConfidence = 0;
		
		if(!IsTracking(user)) {
			return XN_STATUS_OK;
		}
				
		NUI_SKELETON_POSITION_INDEX index = MSRKinectJointMap::GetNuiIndexByXnJoint(eJoint);
		if (index < 0) {
			return XN_STATUS_OK;
		}

		NUI_SKELETON_POSITION_TRACKING_STATE spts = GetSkeletonData(user)->eSkeletonPositionTrackingState[index];
		
		jointOrientation.fConfidence =
			(spts == NUI_SKELETON_POSITION_TRACKED) ? 1.0f :
			(spts == NUI_SKELETON_POSITION_INFERRED) ? 0.5f : 0.0f;

		JointOrientationCalculator joc(this); // fixme should be member
		return joc.GetSkeletonJointOrientation(user, index, jointOrientation);
	}

	virtual XnBool IsTracking(XnUserID user)
	{
		if (user < 1 || user > NUI_SKELETON_COUNT) {
			return FALSE;
		} else {
			return GetSkeletonData(user)->eTrackingState != NUI_SKELETON_NOT_TRACKED;
		}
	}

	virtual XnBool IsCalibrated(XnUserID user)
	{
		return IsTracking(user);
	}

	virtual XnBool IsCalibrating(XnUserID user) { return FALSE; } // always done in a moment
	virtual XnStatus RequestCalibration(XnUserID user, XnBool bForce) { return XN_STATUS_OK; } // ignore
	virtual XnStatus AbortCalibration(XnUserID user) { return XN_STATUS_OK; } // ignore
	virtual XnStatus SaveCalibrationDataToFile(XnUserID user, const XnChar* strFileName) { return XN_STATUS_OK; } // ignore
	virtual XnStatus LoadCalibrationDataFromFile(XnUserID user, const XnChar* strFileName) { return XN_STATUS_OK; } // ignore
	virtual XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot) { return XN_STATUS_OK; } // ignore
	virtual XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot) { return XN_STATUS_OK; } // ignore
	virtual XnStatus ClearCalibrationData(XnUInt32 nSlot) { return XN_STATUS_OK; } // ignore
	virtual XnBool IsCalibrationData(XnUInt32 nSlot) { return FALSE; } // ignore

	virtual XnStatus StartTracking(XnUserID user) { return XN_STATUS_OK; } // ignore

	virtual XnStatus StopTracking(XnUserID user) {
		// FIXME ignore, but make the event happen
		return Reset(user);
	}

	virtual XnStatus Reset(XnUserID user) {
		// FIXME ignore, but make the event happen
		if (IsTracking(user)) {
			OnLostUser(user);
			OnNewUser(user);
		}
		return XN_STATUS_OK;
	}

	virtual XnBool NeedPoseForCalibration()
	{
		return FALSE; // yay!
	}

	virtual XnStatus GetCalibrationPose(XnChar* strPose)
	{
		strcpy(strPose, "");
		return XN_STATUS_OK;
	}

	virtual XnStatus SetSmoothing(XnFloat fSmoothingFactor)
	{
		m_pSkeletonReader->SetSmooth(fSmoothingFactor != 0);
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterCalibrationCallbacks(XnModuleCalibrationStart CalibrationStartCB, XnModuleCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle& hCallback)
	{
		CalibrationCallbackHandleSet* pCallbackHandleSet = new CalibrationCallbackHandleSet();
		if (CalibrationStartCB) m_calibrationStartEvent.Register(CalibrationStartCB, pCookie, &pCallbackHandleSet->hCalibrationStart);
		if (CalibrationEndCB) m_calibrationEndEvent.Register(CalibrationEndCB, pCookie, &pCallbackHandleSet->hCalibrationEnd);
		m_calibrationCallbackHandleSets.push_back(pCallbackHandleSet);
		hCallback = pCallbackHandleSet;
		return XN_STATUS_OK;
	}

	virtual void UnregisterCalibrationCallbacks(XnCallbackHandle hCallback)
	{
		CalibrationCallbackHandleSetList::iterator i = 
			std::find(m_calibrationCallbackHandleSets.begin(), m_calibrationCallbackHandleSets.end(), hCallback);
		if (i != m_calibrationCallbackHandleSets.end()) {
			m_calibrationStartEvent.Unregister((*i)->hCalibrationStart);
			m_calibrationEndEvent.Unregister((*i)->hCalibrationEnd);
			delete *i;
			m_calibrationCallbackHandleSets.erase(i);
		}
	}

	XnStatus RegisterToCalibrationInProgress(XnModuleCalibrationInProgress cb, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromCalibrationInProgress(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

	XnStatus RegisterToCalibrationComplete(XnModuleCalibrationComplete cb, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromCalibrationComplete(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

	XnStatus RegisterToCalibrationStart(XnModuleCalibrationStart cb, void* pCookie, XnCallbackHandle& hCallback)
	{
		// FIXME: ignore for the time being
		return XN_STATUS_OK;
	}

	void UnregisterFromCalibrationStart(XnCallbackHandle hCallback)
	{
		// FIXME: ignore for the time being
	}

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		const NUI_SKELETON_FRAME* pSkeletonFrame = NULL;

		try {
			CHECK_XN_STATUS(SuperClass::UpdateImageData(pFrame, data, lockedRect));
		} catch (XnStatusException& e) {
			return e.nStatus;
		}

		try {
			pSkeletonFrame = m_pSkeletonReader->LockFrame();
			if (pSkeletonFrame) {
				m_skeletonFrame = *pSkeletonFrame;
			}
			m_pSkeletonReader->UnlockFrame();
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			m_pSkeletonReader->UnlockFrame();
			return e.nStatus;
		}
	}

	virtual void OnNewUser(XnUserID userID)
	{
		// raise new user event first
		SuperClass::OnNewUser(userID);

		// mimicks anything necessary before the calibration event
		OnPreCalibration(userID);

		// mimicks calibration event
		m_calibrationStartEvent.Raise(userID);
		m_calibrationEndEvent.Raise(userID, TRUE);
		
		// mimicks anything necessary after the calibration event
		OnPostCalibration(userID);
	}

	virtual void OnPreCalibration(XnUserID userID)
	{
		// nothing to do by default
	}

	virtual void OnPostCalibration(XnUserID userID)
	{
		// nothing to do by default
	}

	virtual void OnLostUser(XnUserID userID)
	{
		// nothing to do here

		// raise lost user event lastly
		SuperClass::OnLostUser(userID);
	}

	inline void copyPointToXn(const Vector4* sp, XnPoint3D* dp)
	{
		static const int SCALE = 1000;

		Vector4 sp2;
		if (m_pReader->IsCalibrateViewPoint()) {
			// FIXME tricky...
#if KINECTSDK_VER >= 100
			USHORT z;
			LONG x, y;
			LONG ix, iy;
			NuiTransformSkeletonToDepthImage(*sp, &x, &y, &z, NUI_IMAGE_RESOLUTION_640x480);
			NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, z, &ix, &iy);
			sp2 = NuiTransformDepthImageToSkeleton(ix, iy, z, NUI_IMAGE_RESOLUTION_640x480);
#else
			USHORT z = USHORT(sp->z * SCALE) << 3;
			FLOAT x, y;
			LONG ix, iy;
			NuiTransformSkeletonToDepthImageF(*sp, &x, &y);
			NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, LONG(x*320), LONG(y*240), z, &ix, &iy);
			sp2 = NuiTransformDepthImageToSkeletonF(ix/640.0f, iy/480.f, z);
#endif
			sp = &sp2;
		}

		dp->X = sp->x * SCALE * m_pReader->GetMirrorFactor();
		dp->Y = sp->y * SCALE;
		dp->Z = sp->z * SCALE;
	}
};
