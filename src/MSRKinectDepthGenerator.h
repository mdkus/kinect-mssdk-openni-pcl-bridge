#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"
#include "MSRKinectStreamAlternativeViewPointCap.h"

class MSRKinectDepthGenerator :
	public virtual AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX>,
	public virtual MSRKinectStreamAlternativeViewPointCap
{
private:
	typedef AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX> SuperClass;

public:
	MSRKinectDepthGenerator();
	virtual ~MSRKinectDepthGenerator();

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual xn::ModuleAlternativeViewPointInterface* GetAlternativeViewPointInterface();
	
	// DepthGenerator methods
	virtual XnDepthPixel* GetDepthMap();
	virtual XnDepthPixel GetDeviceMaxDepth();
	virtual void GetFieldOfView(XnFieldOfView& FOV);
	virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback);

protected:
	virtual MSRKinectStreamContextBase* GetContextBase()
	{
		return m_pReader;
	}

	inline void processPixel(const USHORT* sp, XnDepthPixel* dp)
	{
		*dp = *(dp+1) = *(dp+640) = *(dp+641) = (*sp >> NUI_IMAGE_PLAYER_INDEX_SHIFT);
	}

	virtual XnStatus MSRKinectDepthGenerator::UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect);
};
