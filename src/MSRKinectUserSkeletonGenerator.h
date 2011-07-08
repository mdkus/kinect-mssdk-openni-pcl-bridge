#pragma once
#include "base.h"
#include "MSRKinectUserGenerator.h"
#include "MSRKinectSkeletonReader.h"
#include "MSRKinectJointMap.h"
#include "JointOrientationCalculator.h"

class MSRKinectUserSkeletonGenerator : public virtual MSRKinectUserGenerator, public virtual xn::ModuleSkeletonInterface
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
	XN_DECLARE_EVENT_2ARG(CalibrationEndtEvent, CalibrationEventEndInterface, XnUserID, user, XnBool, isSuccess);

protected:
	MSRKinectSkeletonReader* m_pSkeletonReader;
	NUI_SKELETON_FRAME m_skeletonFrame;

	NUI_SKELETON_DATA* GetSkeletonData(XnUserID user) { return &m_skeletonFrame.SkeletonData[user-1]; };

	CalibrationCallbackHandleSetList m_calibrationCallbackHandleSets;
	CalibrationStartEvent m_calibrationStartEvent;
	CalibrationEndtEvent m_calibrationEndEvent;

public:
	MSRKinectUserSkeletonGenerator()
	{
	}

	virtual XnStatus Init()
	{
		try {
			CHECK_XN_STATUS(SuperClass::Init());

			MSRKinectManager* pMan = MSRKinectManager::getInstance();

			m_pSkeletonReader = pMan->GetSkeletonManager()->GetReader();
			m_pSkeletonReader->Start();

			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			MSRKinectUserGenerator::IsCapabilitySupported(strCapabilityName) ||
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
		strPose = "";
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

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
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
		// mimicks calibration event
		m_calibrationStartEvent.Raise(userID);
		m_calibrationEndEvent.Raise(userID, TRUE);
	}

	virtual void OnLostUser(XnUserID userID)
	{
	}

	inline void copyPointToXn(const Vector4* sp, XnPoint3D* dp)
	{
		static const int SCALE = 1000;

		Vector4 sp2;
		if (m_pReader->IsCalibrateViewPoint()) {
			// FIXME tricky...
			USHORT z = USHORT(sp->z * SCALE) << 3;
			FLOAT x, y;
			LONG ix, iy;
			NuiTransformSkeletonToDepthImageF(*sp, &x, &y);
			NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, LONG(x*320), LONG(y*240), z, &ix, &iy);
			sp2 = NuiTransformDepthImageToSkeletonF(ix/640.0f, iy/480.f, z);
			sp = &sp2;
		}

		dp->X = sp->x * SCALE * m_pReader->GetMirrorFactor();
		dp->Y = sp->y * SCALE;
		dp->Z = sp->z * SCALE;
	}
};
