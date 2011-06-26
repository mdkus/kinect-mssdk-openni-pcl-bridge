#pragma once
#include "base.h"
#include "MSRKinectUserGenerator.h"
#include "MSRKinectSkeletonReader.h"

class MSRKinectUserSkeletonGenerator : public virtual MSRKinectUserGenerator, public virtual xn::ModuleSkeletonInterface
{
private:
	typedef MSRKinectUserGenerator SuperClass;

	class SkeletonListener : public MSRKinectSkeletonReader::Listener
	{
	public:
		MSRKinectUserSkeletonGenerator* outer;

	public:
		virtual void OnUpdateFrame()
		{
			printf("hogehoge!");
		}
	} m_skeletonListener;

protected:
	MSRKinectSkeletonReader* m_pSkeletonReader;

public:
	MSRKinectUserSkeletonGenerator()
	{
		m_skeletonListener.outer = this;
	}

	virtual XnStatus Init()
	{
		try {
			CHECK_XN_STATUS(SuperClass::Init());

			MSRKinectManager* pMan = MSRKinectManager::getInstance();

			m_pSkeletonReader = pMan->GetSkeletonManager()->GetReader();
			m_pSkeletonReader->AddListener(&m_skeletonListener);
			m_pSkeletonReader->Start();

			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return strcmp(strCapabilityName, XN_CAPABILITY_SKELETON) == 0;
	}

	virtual ModuleSkeletonInterface* GetSkeletonInterface() { return this; }

	// Skeleton methods
	virtual XnBool IsJointAvailable(XnSkeletonJoint eJoint) { return FALSE; }
	virtual XnBool IsProfileAvailable(XnSkeletonProfile eProfile) { return FALSE; }
	virtual XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile) { return XN_STATUS_OK; }
	virtual XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState) { return XN_STATUS_OK; }
	virtual XnBool IsJointActive(XnSkeletonJoint eJoint)  { return FALSE; }
	virtual XnStatus RegisterToJointConfigurationChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; }
	virtual void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback) {}
	virtual XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints) { return XN_STATUS_OK; }
	virtual XnStatus GetSkeletonJoint(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation& jointTransformation) { return XN_STATUS_OK; }
	virtual XnStatus GetSkeletonJointPosition(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition& pJointPosition) { return XN_STATUS_OK; }
	virtual XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& pJointOrientation) { return XN_STATUS_OK; }
	virtual XnBool IsTracking(XnUserID user) { return FALSE; }
	virtual XnBool IsCalibrated(XnUserID user) { return FALSE; }
	virtual XnBool IsCalibrating(XnUserID user) { return FALSE; }
	virtual XnStatus RequestCalibration(XnUserID user, XnBool bForce) { return XN_STATUS_OK; }
	virtual XnStatus AbortCalibration(XnUserID user) { return XN_STATUS_OK; }
	virtual XnStatus SaveCalibrationDataToFile(XnUserID user, const XnChar* strFileName) { return XN_STATUS_OK; }
	virtual XnStatus LoadCalibrationDataFromFile(XnUserID user, const XnChar* strFileName) { return XN_STATUS_OK; }
	virtual XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot) { return XN_STATUS_OK; }
	virtual XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot) { return XN_STATUS_OK; }
	virtual XnStatus ClearCalibrationData(XnUInt32 nSlot) { return XN_STATUS_OK; }
	virtual XnBool IsCalibrationData(XnUInt32 nSlot) { return FALSE; }
	virtual XnStatus StartTracking(XnUserID user) { return XN_STATUS_OK; }
	virtual XnStatus StopTracking(XnUserID user) { return XN_STATUS_OK; }
	virtual XnStatus Reset(XnUserID user) { return XN_STATUS_OK; }
	virtual XnBool NeedPoseForCalibration() { return FALSE; }
	virtual XnStatus GetCalibrationPose(XnChar* strPose) { return XN_STATUS_OK; }
	virtual XnStatus SetSmoothing(XnFloat fSmoothingFactor) { return XN_STATUS_OK; }
	virtual XnStatus RegisterCalibrationCallbacks(XnModuleCalibrationStart CalibrationStartCB, XnModuleCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; }
	virtual void UnregisterCalibrationCallbacks(XnCallbackHandle hCallback) {}
};
