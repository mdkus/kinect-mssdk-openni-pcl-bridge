#pragma once
#include "base.h"
#include "MSRKinectDepthGenerator.h"
#include "PrimeSenseProperties.h"

// experimental
class MSRKinectDepthGeneratorCompatibleWithPrimeSense :
	public MSRKinectDepthGenerator
{
private:
	PrimeSenseProperties m_properties;

public:
	MSRKinectDepthGeneratorCompatibleWithPrimeSense()
	{
		m_properties.SetInstancePointer(this);
	}

	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		return m_properties.GetIntProperty(strName, nValue);
	}


	XnStatus GetRealProperty(const XnChar* strName, XnDouble& dValue) const
	{
		return m_properties.GetRealProperty(strName, dValue);
	}


	XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
	{
		return m_properties.GetGeneralProperty(strName, nBufferSize, pBuffer);
	}
};
