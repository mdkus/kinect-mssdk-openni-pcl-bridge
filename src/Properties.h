//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

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
