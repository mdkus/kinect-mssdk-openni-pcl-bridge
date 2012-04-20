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
#include "AbstractMultiThreadFrameGenerator.h"
#include "MSRKinectImageStreamReader.h"
#include "MSRKinectManager.h"
#include "ImageConfiguration.h"
#include "custom_properties.h"

template <class ParentModuleGeneratorClass, class SourcePixelType, class TargetPixelType>
class AbstractMSRKinectImageStreamGenerator :
	public AbstractMultiThreadFrameGenerator<ParentModuleGeneratorClass, MSRKinectImageStreamReader>
{
private:
	typedef AbstractMultiThreadFrameGenerator<ParentModuleGeneratorClass, MSRKinectImageStreamReader> SuperClass;

protected:
	XnPredefinedProductionNodeType m_nodeType;
	ImageConfiguration m_imageConfig;
	TargetPixelType* m_pBuffer;

	XnUInt32 GetXRes() const { return m_imageConfig.GetSelectedMode()->outputMode.nXRes; }
	XnUInt32 GetYRes() const { return m_imageConfig.GetSelectedMode()->outputMode.nYRes; }

protected:
	AbstractMSRKinectImageStreamGenerator(XnPredefinedProductionNodeType nodeType, BOOL bActiveGeneratorControl, const ImageConfiguration::Desc* pImageConfigDesc) :
		SuperClass(bActiveGeneratorControl),
		m_nodeType(nodeType),
		m_imageConfig(pImageConfigDesc),
		m_pBuffer(NULL)
	{
		MSRKinectManager* pMan = MSRKinectManager::GetInstance();
		m_pReader = pMan->GetImageStreamManager(nodeType)->GetReader();
		m_pReader->SetOutputMode(nodeType,  m_imageConfig.GetSelectedMode()->outputMode);
	}

public:
	virtual ~AbstractMSRKinectImageStreamGenerator()
	{
		CleanUpBuffer();
	}

	//
	// Generator methods
	//

	virtual const void* GetData()
	{
		SetUpBuffer();
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

		m_pReader->LockFrame();
		NUI_IMAGE_FRAME *pFrame = m_pReader->GetFrame();
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

	// Custom Properties

	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		if (streq(strName, PROP_COMMON_CAMERA_ELEVATION_ANGLE)) {
			return getCameraElevationAngle(&nValue);
		} else if (streq(strName, PROP_COMMON_NUI_SENSOR_POINTER)) {
			nValue = (XnUInt64) m_pReader->GetSensor();
			return XN_STATUS_OK;
		} else {
			return SuperClass::GetIntProperty(strName, nValue);
		}
	}

	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		if (streq(strName, PROP_COMMON_CAMERA_ELEVATION_ANGLE)) {
			return setCameraElevationAngle(nValue);
		} else {
			return SuperClass::SetIntProperty(strName, nValue);
		}
	}

protected:
	virtual void PreStartGenerating()
	{
		SetUpBuffer();
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const SourcePixelType* data, const NUI_LOCKED_RECT& lockedRect) = 0;

	virtual void SetUpBuffer()
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

	virtual void CleanUpBuffer()
	{
		if (m_pBuffer) {
			delete[] m_pBuffer;
			m_pBuffer = NULL;
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
