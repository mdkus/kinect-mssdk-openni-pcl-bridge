#pragma once
#include "base.h"
#include "MSRKinectDepthGenerator.h"

// experimental
class MSRKinectDepthGeneratorCompatibleWithPrimeSense :
	public MSRKinectDepthGenerator
{
public:
	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		// empirical values
		if (strcmp(strName, "MaxShift") == 0) {
			nValue = 2047;
			return XN_STATUS_OK;
		} else if (strcmp(strName, "ZPD") == 0) {
			nValue = 120;
			return XN_STATUS_OK;
		} else if (strcmp(strName, "ConstShift") == 0) {
			nValue = 200;
			return XN_STATUS_OK;
		}
		return XN_STATUS_ERROR;
	}


	XnStatus GetRealProperty(const XnChar* strName, XnDouble& dValue) const
	{
		// empirical values
		if (strcmp(strName, "ZPPS") == 0) {
			dValue = 0.1052;
			return XN_STATUS_OK;
		} else if (strcmp(strName, "LDDIS") == 0) {
			dValue = 7.5;
			return XN_STATUS_OK;
		}
		return XN_STATUS_ERROR;
	}


	XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
	{
		// todo -- dummy values to allow UserGenerator run
		if (strcmp(strName, "S2D") == 0 || strcmp(strName, "D2S") == 0) {
			return XN_STATUS_OK;
		}
		return XN_STATUS_ERROR;
	}
};
