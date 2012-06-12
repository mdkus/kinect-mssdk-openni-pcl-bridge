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
#include "AbstractMSRKinectImageStreamGenerator.h"

template <class ParentMapGeneratorClass, class SourcePixelType, class TargetPixelType>
class AbstractMSRKinectMapGenerator :
	public AbstractMSRKinectImageStreamGenerator<ParentMapGeneratorClass, SourcePixelType, TargetPixelType>
{
private:
	typedef AbstractMSRKinectImageStreamGenerator<ParentMapGeneratorClass, SourcePixelType, TargetPixelType> SuperClass;
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);


protected:
	ChangeEvent m_mapOutputModeChangeEvent;

protected:
	AbstractMSRKinectMapGenerator(XnPredefinedProductionNodeType nodeType, BOOL bActiveGeneratorControl, const ImageConfiguration::Desc* pImageConfigDesc) :
		 AbstractMSRKinectImageStreamGenerator(nodeType, bActiveGeneratorControl, pImageConfigDesc)
	{
	}

public:
	// MapGenerator methods

	virtual XnUInt32 GetSupportedMapOutputModesCount()
	{
		return m_imageConfig.GetNumberOfSupportedModes();
	}

	virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
	{
		return m_imageConfig.GetSupportedRawModes(aModes, nCount);
	}

	virtual XnStatus SetMapOutputMode(const XnMapOutputMode& mode)
	{
		try {
			CHECK_XN_STATUS(m_imageConfig.SelectRawMode(mode));
			m_pReader->SetOutputMode(m_nodeType, m_imageConfig.GetSelectedMode()->outputMode);
			CleanUpBuffer();
			m_mapOutputModeChangeEvent.Raise();
			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			return e.nStatus;
		}
	}

	virtual XnStatus GetMapOutputMode(XnMapOutputMode& mode)
	{
		mode = m_imageConfig.GetSelectedMode()->outputMode;
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		m_mapOutputModeChangeEvent.Register(handler, pCookie, &hCallback);
		return XN_STATUS_OK;
	}

	virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
	{
		m_mapOutputModeChangeEvent.Unregister(hCallback);
	}

protected:
	void PopulateMapMetaData(XnMapMetaData* pMetaData)
	{
		XnMapOutputMode mode;
		GetMapOutputMode(mode);

		// assume we don't support cropping
		pMetaData->FullRes.X = pMetaData->Res.X = mode.nXRes;
		pMetaData->FullRes.Y = pMetaData->Res.Y = mode.nYRes;
		pMetaData->nFPS = mode.nFPS;
		pMetaData->Offset.X = pMetaData->Offset.Y = 0;

		// get the data for frame
		XnUInt64 dummyTimestamp;
		pMetaData->pOutput->bIsNew = IsNewDataAvailable(dummyTimestamp); // I don't know if this is OK
		pMetaData->pOutput->nDataSize = GetDataSize();
		pMetaData->pOutput->nFrameID = GetFrameID();
		pMetaData->pOutput->nTimestamp = GetTimestamp();
	}
};
