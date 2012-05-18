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

template <class ParentModuleGeneratorClass>
class AbstractModuleGenerator : public ParentModuleGeneratorClass
{
private:
	typedef ParentModuleGeneratorClass SuperClass;
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

protected:
	ChangeEvent m_newDataAvailableEvent;
	ChangeEvent m_generationRunningChangeEvent;
	std::string m_nodeName;
	XnNodeNotifications* m_pNodeNotifications;
	void* m_pNodeNotificationsdCookie;

public:
	AbstractModuleGenerator() : m_pNodeNotifications(NULL)
	{
	}

	virtual ~AbstractModuleGenerator()
	{
	}

	void SetNodeName(const XnChar* name)
	{
		m_nodeName = name;
	}

	const XnChar* GetNodeName()
	{
		return m_nodeName.c_str();
	}

	//
	// Generator methods
	//

	virtual XnStatus RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return m_newDataAvailableEvent.Register(handler, pCookie, &hCallback);
	}

	virtual void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
	{
		m_newDataAvailableEvent.Unregister(hCallback);
	}

	virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return m_generationRunningChangeEvent.Register(handler, pCookie, &hCallback);
	}
	
	virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
	{
		m_generationRunningChangeEvent.Unregister(hCallback);
	}

protected:
	//
	// Property change notifications
	//
	void RegisterNodeNotifications(XnNodeNotifications* pNotifications, void* pCookie)
	{
		m_pNodeNotifications = pNotifications;
		m_pNodeNotificationsdCookie = pCookie;
	}

	void UnregisterNodeNotifications()
	{
	}

	XnStatus OnNodeIntPropChanged(const XnChar* strPropName, XnUInt64 nValue)
	{
		if (m_pNodeNotifications) {
			return m_pNodeNotifications->OnNodeIntPropChanged(m_pNodeNotificationsdCookie, m_nodeName.c_str(), strPropName, nValue);
		} else {
			return XN_STATUS_OK;
		}
	}

	XnStatus OnNodeRealPropChanged(const XnChar* strPropName, XnDouble dValue)
	{
		if (m_pNodeNotifications) {
			return m_pNodeNotifications->OnNodeRealPropChanged(m_pNodeNotificationsdCookie, m_nodeName.c_str(), strPropName, dValue);
		} else {
			return XN_STATUS_OK;
		}
	}


	XnStatus OnNodeStringPropChanged(const XnChar* strPropName, const XnChar* strValue)
	{
		if (m_pNodeNotifications) {
			return m_pNodeNotifications->OnNodeStringPropChanged(m_pNodeNotificationsdCookie, m_nodeName.c_str(), strPropName, strValue);
		} else {
			return XN_STATUS_OK;
		}
	}

	XnStatus OnNodeGeneralPropChanged(const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
	{
		if (m_pNodeNotifications) {
			return m_pNodeNotifications->OnNodeGeneralPropChanged(m_pNodeNotificationsdCookie, m_nodeName.c_str(), strPropName, nBufferSize, pBuffer);
		} else {
			return XN_STATUS_OK;
		}
	}

};
