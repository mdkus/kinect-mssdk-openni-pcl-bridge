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
#include "MSRKinectImageStreamReader.h"
#include "MSRKinectManager.h"
#include "ImageConfiguration.h"

template <class ParentModuleGeneratorClass, class SourcePixelType, class TargetPixelType>
class AbstractMSRKinectImageStreamGenerator :
	public virtual ParentModuleGeneratorClass,
	public virtual MSRKinectImageStreamReader::Listener
{
private:
	typedef ParentModuleGeneratorClass SuperClass;
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

private:
	ChangeEvent m_dataAvailableEvent;

protected:
	XnPredefinedProductionNodeType m_nodeType;
	BOOL m_bActiveGeneratorControl;
	ImageConfiguration m_imageConfig;

	MSRKinectImageStreamReader* m_pReader;
	BOOL m_bNewDataAvailable;
	TargetPixelType* m_pBuffer;

	XnUInt32 GetXRes() const { return m_imageConfig.GetSelectedMode()->outputMode.nXRes; }
	XnUInt32 GetYRes() const { return m_imageConfig.GetSelectedMode()->outputMode.nYRes; }

protected:
	AbstractMSRKinectImageStreamGenerator(XnPredefinedProductionNodeType nodeType, BOOL bActiveGeneratorControl, const ImageConfiguration::Desc* pImageConfigDesc) :
		m_nodeType(nodeType),
		m_bActiveGeneratorControl(bActiveGeneratorControl),
		m_imageConfig(pImageConfigDesc),
		m_pBuffer(NULL),
		m_bNewDataAvailable(FALSE)
	{
		MSRKinectManager* pMan = MSRKinectManager::GetInstance();
		m_pReader = pMan->GetImageStreamManager(nodeType)->GetReader();
		m_pReader->SetOutputMode(nodeType,  m_imageConfig.GetSelectedMode()->outputMode);
	}

public:
	virtual ~AbstractMSRKinectImageStreamGenerator()
	{
		clearBuffer();

		if (m_pReader)
		{
			m_pReader->RemoveListener(this);
		}
	}

	virtual void OnUpdateFrame() {
		m_bNewDataAvailable = TRUE;
		m_dataAvailableEvent.Raise();
	}

	//
	// Generator methods
	//

	virtual XnStatus RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return m_dataAvailableEvent.Register(handler, pCookie, &hCallback);
	}

	virtual void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
	{
		m_dataAvailableEvent.Unregister(hCallback);
	}

	virtual XnBool IsNewDataAvailable(XnUInt64& nTimestamp)
	{
		return m_bNewDataAvailable;
	}

	virtual const void* GetData()
	{
		setupBuffer();
		return m_pBuffer;
	}

	virtual XnUInt32 GetDataSize()
	{
		return GetXRes() * GetYRes() * sizeof(TargetPixelType);
	}

	virtual XnStatus UpdateData()
	{
		if (!m_bNewDataAvailable) {
			return XN_STATUS_OK;
		}

		const NUI_IMAGE_FRAME *pFrame = m_pReader->LockFrame();
		if (pFrame) {
			NUI_LOCKED_RECT lockedRect;
			pFrame->pFrameTexture->LockRect(0, &lockedRect, NULL, 0);

			// FIXME check status code
			UpdateImageData(pFrame, (SourcePixelType*)lockedRect.pBits, lockedRect);

			m_bNewDataAvailable = FALSE;
		} else {
			// keep the previous result
		}
		m_pReader->UnlockFrame();

		return XN_STATUS_OK;
	}

	virtual XnUInt64 GetTimestamp()
	{
		return m_pReader->GetTimestamp();
	}

	virtual XnUInt32 GetFrameID()
	{
		return m_pReader->GetFrameID();
	}

	//
	// Generator Start/Stop
	//

	virtual XnBool IsGenerating()
	{
		return m_pReader->IsRunning();
	}
	
	virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return m_pReader->GetGeneratingEvent()->Register(handler, pCookie, &hCallback);
	}
	
	virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
	{
		m_pReader->GetGeneratingEvent()->Unregister(hCallback);
	}

	virtual XnStatus StartGenerating()
	{
		try {
			setupBuffer();

			m_pReader->AddListener(this);
			if (m_bActiveGeneratorControl) {
				CHECK_XN_STATUS(m_pReader->Start());
			}
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}
	
	virtual void StopGenerating()
	{
		if (m_bActiveGeneratorControl) {
			m_pReader->Stop();
		}
		m_pReader->RemoveListener(this);
	}

	// Custom Properties

	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		if (strcmp(strName, PROP_COMMON_CAMERA_ELEVATION_ANGLE) == 0) {
			return getCameraElevationAngle(&nValue);
		} else if (strcmp(strName, PROP_COMMON_NUI_SENSOR_POINTER) == 0) {
			nValue = (XnUInt64) m_pReader->GetSensor();
			return XN_STATUS_OK;
		} else {
			return SuperClass::GetIntProperty(strName, nValue);
		}
	}

	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		if (strcmp(strName, PROP_COMMON_CAMERA_ELEVATION_ANGLE) == 0) {
			return setCameraElevationAngle(nValue);
		} else {
			return SuperClass::SetIntProperty(strName, nValue);
		}
	}

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const SourcePixelType* data, const NUI_LOCKED_RECT& lockedRect) = 0;

	void clearBuffer()
	{
		if (m_pBuffer) {
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}
	}

	void setupBuffer()
	{
		if (!m_pBuffer) {
			// lazily initialize the buffer because the size is unknown at Init
			m_pBuffer = new TargetPixelType[GetXRes() * GetYRes()];
			if (m_pBuffer == NULL) {
				throw new XnStatusException(XN_STATUS_ALLOC_FAILED);
			}
			xnOSMemSet(m_pBuffer, 0, GetXRes() * GetYRes() * sizeof(TargetPixelType));
		}
	}

private:
	XnStatus getCameraElevationAngle(XnUInt64* pValue) const
	{
		INuiSensor* pSensor = m_pReader->GetSensor();
		if (pSensor) {
			LONG angle;
			HRESULT hr = pSensor->NuiCameraElevationGetAngle(&angle);
			if (SUCCEEDED(hr)) {
				*pValue = angle;
				return XN_STATUS_OK;
			} else {
				return XN_STATUS_ERROR;
			}
		} else {
			return XN_STATUS_NOT_INIT;
		}
	}

	XnStatus setCameraElevationAngle(XnUInt64 value)
	{
		INuiSensor* pSensor = m_pReader->GetSensor();
		if (pSensor) {
			HRESULT hr = pSensor->NuiCameraElevationSetAngle((LONG) value);
			return (SUCCEEDED(hr)) ? XN_STATUS_OK : XN_STATUS_ERROR;
		} else {
			return XN_STATUS_NOT_INIT;
		}
	}
};
