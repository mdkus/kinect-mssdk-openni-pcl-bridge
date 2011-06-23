#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"

class MSRKinectDepthGenerator :
	public virtual AbstractMSRKinectMapGenerator<xn::ModuleDepthGenerator, USHORT, XnDepthPixel, NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240>,
	public virtual xn::ModuleAlternativeViewPointInterface
{
private:
	BOOL m_bCalibrateViewPoint;

	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);
	ChangeEvent m_viewPointChangeEvent;

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

	// AlternativeViewPoint method
	virtual XnBool IsViewPointSupported(xn::ProductionNode& other);
	virtual XnStatus SetViewPoint(xn::ProductionNode& other);
	virtual XnBool IsViewPointAs(xn::ProductionNode& other);
	virtual XnStatus ResetViewPoint();
	virtual XnStatus RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromViewPointChange(XnCallbackHandle hCallback);

protected:
	virtual XnStatus MSRKinectDepthGenerator::UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const KINECT_LOCKED_RECT& lockedRect);
};
