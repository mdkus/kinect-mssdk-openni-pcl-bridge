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
// Contributor: Michael Dingerkus <mdkus@web.de>,  Copyright (c) 2013
//@COPYRIGHT@//

#pragma once
#include "base.h"
#include "version.h"

static XnProductionNodeDescription descDevice;
static std::string connectionInfo;

template<class ProductionNodeClass>
class ProductionNodeExporter {
private:
	XnPredefinedProductionNodeType m_nodeType;
	const XnChar* m_strName;

public:
	ProductionNodeExporter(const XnChar* strName, XnPredefinedProductionNodeType nodeType)
	{
		m_nodeType = nodeType;
		m_strName = strName;
	}

	virtual ~ProductionNodeExporter()
	{
	}

	void GetDescription(XnProductionNodeDescription* pDescription)
	{
		pDescription->Type = m_nodeType;
		if (m_nodeType == XN_NODE_TYPE_DEPTH || m_nodeType == XN_NODE_TYPE_IMAGE)
			strcpy(pDescription->strVendor, VERSION_VENDOR_PCL);
		else
			strcpy(pDescription->strVendor, VERSION_VENDOR);
		strcpy(pDescription->strName, m_strName);
		pDescription->Version.nMajor = VERSION_MAJOR;
		pDescription->Version.nMinor = VERSION_MINOR;
		pDescription->Version.nMaintenance = VERSION_MAINTENANCE;
		pDescription->Version.nBuild = VERSION_BUILD;
	}

	XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& nodes, xn::EnumerationErrors* pErrors)
	{
		return EnumerateProductionTreesImpl(context, nodes, pErrors);
	}

	XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNodes, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
	{
		return CreateImpl(context, strInstanceName, strCreationInfo, pNodes, strConfigurationDir, ppInstance);
	}

	void Destroy(xn::ModuleProductionNode* pInstance)
	{
		delete pInstance;
	}

protected:
	virtual XnStatus CreateImpl(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNodes, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance) = 0;
	virtual XnStatus EnumerateProductionTreesImpl(xn::Context& context, xn::NodeInfoList& nodes, xn::EnumerationErrors* pErrors) = 0;

};
