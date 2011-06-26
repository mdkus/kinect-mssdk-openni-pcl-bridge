#pragma once
#include "base.h"
#include "MSRKinectDepthGeneratorBase.h"
#include "MSRKinectGeneratorControls.h"
#include "MSRKinectMirrorCap.h"
#include "MSRKinectAlternativeViewPointCap.h"

class MSRKinectDepthGeneratorDepthPixelProcessor
{
public:
	static void Process(const USHORT* sp, XnDepthPixel* dp)
	{
		*dp = *(dp+1) = *(dp+640) = *(dp+641) = (*sp >> NUI_IMAGE_PLAYER_INDEX_SHIFT);
	}
};

class MSRKinectDepthGenerator :
	public virtual MSRKinectDepthGeneratorBase<xn::ModuleDepthGenerator, MSRKinectDepthGeneratorDepthPixelProcessor>,
	public virtual MSRKinectMirrorCap,
	public virtual MSRKinectAlternativeViewPointCap
{
private:
	typedef MSRKinectDepthGeneratorBase<xn::ModuleDepthGenerator, MSRKinectDepthGeneratorDepthPixelProcessor> SuperClass;

public:
	MSRKinectDepthGenerator() {}
	virtual ~MSRKinectDepthGenerator() {}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			MSRKinectMirrorCap::IsCapabilitySupported(strCapabilityName) ||
			MSRKinectAlternativeViewPointCap::IsCapabilitySupported(strCapabilityName);
	}

	// DepthGenerator methods
	virtual XnDepthPixel* GetDepthMap() { return m_pBuffer; }

	virtual XnDepthPixel GetDeviceMaxDepth() { return NUI_IMAGE_DEPTH_MAXIMUM >> NUI_IMAGE_PLAYER_INDEX_SHIFT; }

	virtual void GetFieldOfView(XnFieldOfView& fov)
	{
		// empirical value
		fov.fHFOV = 1.0145;
		fov.fVFOV = 0.7898;
	}

	virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) { return XN_STATUS_OK; } // ignore
	virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback) {} // ignore

	MSRKinectAlternativeViewPointCap_IMPL(m_pReader);
	MSRKinectMirrorCap_IMPL(m_pReader);
	ActiveMSRKinectGeneratorControl_IMPL(m_pReader);

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
	{
		return UpdateDepthData(MSRKinectDepthGeneratorDepthPixelProcessor(), pFrame, data, lockedRect);
	}
};
