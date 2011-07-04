#pragma once
#include "base.h"
#include <string>
#include <map>

class Properties
{
public:
	struct Binary {
		XnUInt32 length;
		const BYTE* data;
	};

private:
	std::map<std::string, XnUInt64> m_intProperties;
	std::map<std::string, XnDouble> m_realProperties;
	std::map<std::string, Binary> m_generalProperties;

public:
	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		std::map<std::string, XnUInt64>::const_iterator p = m_intProperties.find(std::string(strName));
		if (p != m_intProperties.end()) {
			nValue = p->second;
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_BAD_PARAM;
		}
	}

	XnStatus GetRealProperty(const XnChar* strName, XnDouble& dValue) const
	{
		std::map<std::string, XnDouble>::const_iterator p = m_realProperties.find(std::string(strName));
		if (p != m_realProperties.end()) {
			dValue = p->second;
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_BAD_PARAM;
		}
	}

	XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
	{
		std::map<std::string, Binary>::const_iterator p = m_generalProperties.find(strName);
		if (p != m_generalProperties.end()) {
			xnOSMemCopy(pBuffer, p->second.data, min(p->second.length, nBufferSize));
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_BAD_PARAM;
		}
	}

protected:
	void SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		m_intProperties[strName] = nValue;
	}

	void SetRealProperty(const XnChar* strName, XnDouble dValue)
	{
		m_realProperties[strName] = dValue;
	}

	void SetGeneralProperty(const XnChar* strName, XnUInt32 length, const BYTE* data)
	{
		// buffer must be retained
		Binary value;
		value.length = length;
		value.data = data;
		m_generalProperties[strName] = value;
	}
};
