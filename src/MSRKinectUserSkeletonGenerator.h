#pragma once
#include "base.h"
#include "MSRKinectUserGenerator.h"
#include "MSRKinectSkeletonReader.h"
#include "MSRKinectJointMap.h"

class MSRKinectUserSkeletonGenerator : public virtual MSRKinectUserGenerator, public virtual xn::ModuleSkeletonInterface
{
private:
	typedef MSRKinectUserGenerator SuperClass;

	//class SkeletonListener : public MSRKinectSkeletonReader::Listener
	//{
	//public:
	//	MSRKinectUserSkeletonGenerator* outer;

	//public:
	//	virtual void OnUpdateFrame()
	//	{
	//		// Just copy. Do not fire event.
	//		outer->m_frame = *outer->m_pSkeletonReader->LockFrame();
	//		outer->m_pSkeletonReader->ReleaseFrame();
	//	}
	//} m_skeletonListener;

protected:
	MSRKinectSkeletonReader* m_pSkeletonReader;
	//Vector4 m_coms[NUI_SKELETON_COUNT];
	//XnSkeletonJointPosition m_joints[NUI_SKELETON_COUNT][30];
	NUI_SKELETON_FRAME m_skeletonFrame;

	NUI_SKELETON_DATA* GetSkeletonData(XnUserID user) { return &m_skeletonFrame.SkeletonData[user-1]; };

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
			//m_pSkeletonReader->AddListener(&m_skeletonListener);
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
			const Vector4& p = GetSkeletonData(user)->Position;
			com.X = p.x;
			com.Y = p.y;
			com.Z = p.z;
			// fprintf(stderr, "CoM=[%f %f %f]\n", p.x, p.y, p.z);
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

	virtual XnBool IsProfileAvailable(XnSkeletonProfile eProfile)
	{
		return TRUE; // probably works
	}

	virtual XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual XnBool IsJointActive(XnSkeletonJoint eJoint) 
	{
		return IsJointAvailable(eJoint);
	}

	virtual XnStatus RegisterToJointConfigurationChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback)
	{
		// ignore
	}

	virtual XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints)
	{
		return XN_STATUS_OK; // ignore
	}

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

		const Vector4* p = &GetSkeletonData(user)->SkeletonPositions[index];
		jointPosition.position.X = p->x;
		jointPosition.position.Y = p->y;
		jointPosition.position.Z = p->z;

		// fprintf(stderr, "Joint=[%f %f %f]\n", p->x, p->y, p->z);

		return XN_STATUS_OK;
	}

	virtual XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& jointOrientation)
	{
		xnOSMemSet(&jointOrientation, 0, sizeof(jointOrientation)); // not supported
		return XN_STATUS_OK;
	}

	virtual XnBool IsTracking(XnUserID user)
	{
		if (user < 1 || user > NUI_SKELETON_COUNT) {
			return FALSE;
		} else {
			return GetSkeletonData(user)->eTrackingState != NUI_SKELETON_NOT_TRACKED;
		}
	}

	virtual XnBool IsCalibrated(XnUserID user) { return FALSE; }

	virtual XnBool IsCalibrating(XnUserID user) { return FALSE; }

	virtual XnStatus RequestCalibration(XnUserID user, XnBool bForce) { return XN_STATUS_OK; }

	virtual XnStatus AbortCalibration(XnUserID user) { return XN_STATUS_OK; }

	virtual XnStatus SaveCalibrationDataToFile(XnUserID user, const XnChar* strFileName) { return XN_STATUS_OK; }

	virtual XnStatus LoadCalibrationDataFromFile(XnUserID user, const XnChar* strFileName) { return XN_STATUS_OK; }

	virtual XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual XnStatus ClearCalibrationData(XnUInt32 nSlot)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual XnBool IsCalibrationData(XnUInt32 nSlot)
	{
		return FALSE; // ignore
	}

	virtual XnStatus StartTracking(XnUserID user)
	{
		return XN_STATUS_OK; // ignore
	}

	virtual XnStatus StopTracking(XnUserID user)
	{
		return XN_STATUS_OK; // ignore
	}
	
	virtual XnStatus Reset(XnUserID user)
	{
		return XN_STATUS_OK; // ignore
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
		return XN_STATUS_OK; // ignore
	}

	virtual void UnregisterCalibrationCallbacks(XnCallbackHandle hCallback)
	{
		// ignore
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
			//	for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
			//		const NUI_SKELETON_DATA* data = pSkeletonFrame->SkeletonData + i;
			//		switch (data->eTrackingState) {
			//		case NUI_SKELETON_TRACKED:
			//			UpdateJoints(i+1, data);
			//			// fall down
			//		case NUI_SKELETON_POSITION_ONLY:
			//			UpdateCoM(i+1, data);
			//			break;
			//		default:
			//		}
			//	}
			}
			m_pSkeletonReader->UnlockFrame();
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			m_pSkeletonReader->UnlockFrame();
			return e.nStatus;
		}
	}
};
