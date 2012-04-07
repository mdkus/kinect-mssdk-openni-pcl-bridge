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

#define PROP_DISTINCT_OVERFLOW_DEPTH_VALUES "distinctOverflowDepthValues"
#define PROP_NEAR_MODE "nearMode"

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

	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		if (strcmp(strName, PROP_DISTINCT_OVERFLOW_DEPTH_VALUES) == 0) {
			nValue = m_pReader->GetRequirement()->m_distinctDepthValues;
			return XN_STATUS_OK;
		} else if (strcmp(strName, PROP_NEAR_MODE) == 0) {
			nValue = m_pReader->GetRequirement()->m_nearMode;
			return XN_STATUS_OK;
		} else {
			return SuperClass::GetIntProperty(strName, nValue);
		}
	}

	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		if (strcmp(strName, PROP_DISTINCT_OVERFLOW_DEPTH_VALUES) == 0) {
			m_pReader->GetRequirement()->m_distinctDepthValues = !!nValue;
			return XN_STATUS_OK;
		} else if (strcmp(strName, PROP_NEAR_MODE) == 0) {
			m_pReader->GetRequirement()->m_nearMode = !!nValue;
			return XN_STATUS_OK;
		} else {
			return SuperClass::SetIntProperty(strName, nValue);
		}
	}

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		return UpdateDepthData(MSRKinectDepthGeneratorDepthPixelProcessor(), pFrame, data, lockedRect);
	}
};
