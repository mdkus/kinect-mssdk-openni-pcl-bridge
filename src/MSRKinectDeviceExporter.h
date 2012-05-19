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
#include "util.h"
#include "ProductionNodeExporter.h"
#include "MSRKinectDevice.h"

class MSRKinectDeviceExporter : public ProductionNodeExporter<MSRKinectDevice>
{
public:
	MSRKinectDeviceExporter() : ProductionNodeExporter("KinectSDKDevice", XN_NODE_TYPE_DEVICE) {}

protected:
	virtual XnStatus CreateImpl(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNodes, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
	{
		try {
			MSRKinectDevice* node = new MSRKinectDevice(strCreationInfo);
			node->SetNodeName(strInstanceName);
			*ppInstance = node;
			return XN_STATUS_OK;
		} catch (XnStatusException e) {
			return e.nStatus;
		}

	}

	virtual XnStatus EnumerateProductionTreesImpl(xn::Context& context, xn::NodeInfoList& nodes, xn::EnumerationErrors* pErrors)
	{
		int count;
		CHECK_HRESULT(NuiGetSensorCount(&count));
		if (count == 0) {
			return XN_STATUS_DEVICE_NOT_CONNECTED;
		}

		XnProductionNodeDescription desc;
		GetDescription(&desc);
		NodeInfoList neededNodes;

		for (int i = 0; i < count; i++) {
			INuiSensor* pSensor = NULL;
			CHECK_HRESULT(NuiCreateSensorByIndex(i, &pSensor));
			nodes.Add(desc, bstr2cstr(pSensor->NuiDeviceConnectionId()).c_str(), &neededNodes);
			pSensor->Release();
		}

		return XN_STATUS_OK;
	}
};

XN_EXPORT_DEVICE(MSRKinectDeviceExporter);
