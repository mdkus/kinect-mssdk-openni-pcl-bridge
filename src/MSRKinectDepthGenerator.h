#pragma once
#include "base.h"
#include "MSRKinectDepthGeneratorBase.h"
#include "MSRKinectMirrorCap.h"
#include "MSRKinectAlternativeViewPointCap.h"

class MSRKinectDepthGeneratorDepthPixelProcessor
{
public:
	void Process(USHORT d, XnDepthPixel* dp, XnInt32 ratio)
	{
		d >>= NUI_IMAGE_PLAYER_INDEX_SHIFT;
		*dp = d;
		if (ratio == 2) {
			*(dp+1) = *(dp+640) = *(dp+641) = d;
		}
	}
};

class MSRKinectDepthGenerator :
	public MSRKinectDepthGeneratorBase<xn::ModuleDepthGenerator, MSRKinectDepthGeneratorDepthPixelProcessor>,
	public virtual MSRKinectMirrorCap,
	public virtual MSRKinectAlternativeViewPointCap
{
private:
	typedef MSRKinectDepthGeneratorBase SuperClass;

public:
	MSRKinectDepthGenerator() : SuperClass(XN_NODE_TYPE_DEPTH, TRUE)
	{
	}

	virtual ~MSRKinectDepthGenerator() {}

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return
			MSRKinectMirrorCap::IsCapabilitySupported(strCapabilityName) ||
			MSRKinectAlternativeViewPointCap::IsCapabilitySupported(strCapabilityName);
	}

	// DepthGenerator methods
	virtual XnDepthPixel* GetDepthMap()
	{
		setupBuffer();
		return m_pBuffer;
	}

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

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect)
	{
		return UpdateDepthData(MSRKinectDepthGeneratorDepthPixelProcessor(), pFrame, data, lockedRect);
	}
};
