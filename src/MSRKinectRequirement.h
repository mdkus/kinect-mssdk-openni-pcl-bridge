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
#include "MSRKinectState.h"
#include "ConnectionInfoUtil.h"

class MSRKinectRequirement
{
private:
	DWORD m_nInitFlags;
	NUI_IMAGE_RESOLUTION m_colorImageResolution;
	NUI_IMAGE_RESOLUTION m_depthImageResolution;
	BOOL m_bInitialized;
	std::string m_requiredSensorID;

	MSRKinectState m_state;
	MSRKinectState* m_pState; // I don't like having state here. Refactoring required.

public:
	MSRKinectRequirement() :
		m_nInitFlags(0),
		m_colorImageResolution(NUI_IMAGE_RESOLUTION_INVALID),
		m_depthImageResolution(NUI_IMAGE_RESOLUTION_INVALID),
		m_bInitialized(FALSE),
		m_pState(&m_state)
	{
	}

	void AddRequirement(XnPredefinedProductionNodeType nodeType, const XnMapOutputMode& outputMode)
	{
		switch (nodeType) {
		case XN_NODE_TYPE_IMAGE:
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;

			// Lastly specified resolution wins
			if (outputMode.nXRes == 640 && outputMode.nYRes == 480) {
				m_colorImageResolution = NUI_IMAGE_RESOLUTION_640x480;
#if KINECTSDK_VER >= 100
			} else if (outputMode.nXRes ==  1280 && outputMode.nYRes == 960) {
				m_colorImageResolution = NUI_IMAGE_RESOLUTION_1280x960;
#else
			} else if (outputMode.nXRes ==  1280 && outputMode.nYRes == 1024) {
				m_colorImageResolution = NUI_IMAGE_RESOLUTION_1280x1024;
#endif
			}
			break;

		case XN_NODE_TYPE_DEPTH:
			// We cannot open both NUI_IMAGE_TYPE_DEPTH and NUI_IMAGE_TYPE_COLOR_IN_DEPTH_SPACE at the sametime.
			if (!(m_nInitFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX)) {
				m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
			}

			// Lastly specified resolution wins
			if (outputMode.nXRes == 640 && outputMode.nYRes == 480) {
				m_depthImageResolution = NUI_IMAGE_RESOLUTION_640x480;
			} else if (outputMode.nXRes ==  320 && outputMode.nYRes == 240) {
				m_depthImageResolution = NUI_IMAGE_RESOLUTION_320x240;
			}
			break;

		case XN_NODE_TYPE_USER:
			// Assume we cannot or should not open NUI_INITIALIZE_FLAG_USES_DEPTH at the same time
			m_nInitFlags &= ~NUI_INITIALIZE_FLAG_USES_DEPTH;
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;

			// Ignore the resolution
			break;
		}
	}

	void AddRequirement(XnPredefinedProductionNodeType nodeType)
	{
		switch (nodeType) {
		case XN_NODE_TYPE_AUDIO:
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_AUDIO;
			break;
		}
	}

	void AddCapabilityRequirement(const char* strCapability)
	{
		if (streq(strCapability, XN_CAPABILITY_SKELETON)) {
			m_nInitFlags |= NUI_INITIALIZE_FLAG_USES_SKELETON;
		}
	}

	DWORD GetInitFlags() const
	{
		return m_nInitFlags;
	}

	NUI_IMAGE_RESOLUTION GetColorImageResolution() const
	{
		return m_colorImageResolution;
	}

	BOOL IsUserNodeRequired() const
	{
		return m_nInitFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
	}

	NUI_IMAGE_RESOLUTION GetDepthImageResolution() const
	{
		return IsUserNodeRequired() ? NUI_IMAGE_RESOLUTION_320x240 : m_depthImageResolution;
	}

	NUI_IMAGE_TYPE GetDepthImageType() const
	{
		return IsUserNodeRequired() ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH;
	}

	void DoInitialize() // throws XnStatusException
	{
		if (m_bInitialized) return;

		m_pState->SetSensor(findFirstAvailableSensor());
		m_bInitialized = TRUE;
	}

	void DoShutdown()
	{
		m_pState->ReleaseSensor();
		// m_bInitialized = FALSE;
	}

	INuiSensor* GetSensor()
	{
		return m_pState->GetSensor();
	}

	MSRKinectState* GetState()
	{
		return m_pState;
	}

	// Specify the sensor ID to use.
	void RequireSensorID(const XnChar* id)
	{
		m_requiredSensorID = id;
	}

private:
	INuiSensor* findFirstAvailableSensor()
	{
		// TODO: move to somewhere else

		// Register the device status callback.
		// Caution: This may conflict with the application's device callback.
		NuiSetDeviceStatusCallback(deviceStatusCallback, this);

		int count;
		CHECK_HRESULT(NuiGetSensorCount(&count));
		if (count == 0) {
			throw XnStatusException(XN_STATUS_DEVICE_NOT_CONNECTED);
		}

		INuiSensor* pSensor = NULL;
		if (m_requiredSensorID.empty()) {
			for (int i = 0; i < count; i++) {
				CHECK_HRESULT(NuiCreateSensorByIndex(i, &pSensor));

				HRESULT hr = pSensor->NuiInitialize(m_nInitFlags);
				if (FAILED(hr)) 
				{
					pSensor->Release();
					pSensor = NULL;
					if (i < count - 1) {
						continue; // retry
					} else {
						CHECK_HRESULT(hr); // throw an exception because this was the last one
					}
				}
				break; // got one
			}
		} else {
			BSTR id = cstr2bstr(ConnectionInfoUtil::decodeConnectionInfo(m_requiredSensorID.c_str()).c_str());
			CHECK_HRESULT(NuiCreateSensorById(id, &pSensor));
			SysFreeString(id);
			CHECK_HRESULT(pSensor->NuiInitialize(m_nInitFlags));
		}

		return pSensor;
	}

	static void CALLBACK deviceStatusCallback(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR*, void* pUserData)
	{
		// TODO: move to somewhere else

		MSRKinectRequirement* pRequirement = (MSRKinectRequirement*) pUserData;

		if ( SUCCEEDED( hrStatus ) ) {
			wprintf(L"Kinect-Sensor Connected (%s)\n", instanceName); // Only for information
		} else {
			wprintf(L"Kinect-Sensor Disconnected (%s)\n", instanceName);
			if (wstreq(instanceName, pRequirement->GetSensor()->NuiDeviceConnectionId())) {
				printf("Instance name matched. Shutdown the sensor.\n");
				pRequirement->DoShutdown();
			}
		}
	}
};
