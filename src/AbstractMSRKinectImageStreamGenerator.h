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
	typedef AbstractMSRKinectImageStreamGenerator<ParentModuleGeneratorClass, SourcePixelType, TargetPixelType> ThisClass;
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

private:
	ChangeEvent m_dataAvailableEvent;

protected:
	XnPredefinedProductionNodeType m_nodeType;
	ImageConfiguration m_imageConfig;
	BOOL m_bActiveGeneratorControl;
	MSRKinectImageStreamReader* m_pReader;
	BOOL m_bNewDataAvailable;
	TargetPixelType* m_pBuffer;

	XnUInt32 GetXRes() const { return m_imageConfig.GetSelectedMode()->outputMode.nXRes; }
	XnUInt32 GetYRes() const { return m_imageConfig.GetSelectedMode()->outputMode.nYRes; }

protected:
	AbstractMSRKinectImageStreamGenerator() :
		m_pBuffer(NULL),
		m_bNewDataAvailable(FALSE),
		m_bActiveGeneratorControl(TRUE)
	{
	}

	void SetNodeType(XnPredefinedProductionNodeType nodeType)
	{
		m_nodeType = nodeType;
	}

	void SetImageConfigurationDesc(const ImageConfiguration::Desc* pDesc)
	{
		m_imageConfig.SetDesc(pDesc);
	}

	void SetActiveGeneratorControl(BOOL value)
	{
		m_bActiveGeneratorControl = value;
	}

public:
	virtual ~AbstractMSRKinectImageStreamGenerator()
	{
		if (m_pBuffer) {
			delete[] m_pBuffer;
		}
	}

	virtual XnStatus Init()
	{
		try {
			MSRKinectManager* pMan = MSRKinectManager::GetInstance();

			m_pReader = pMan->GetImageStreamManager(m_nodeType)->GetReader();
			m_pReader->SetOutputMode(m_nodeType,  m_imageConfig.GetSelectedMode()->outputMode);

			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			m_pReader->RemoveListener(this);
			return e.nStatus;
		}
	}

	virtual void OnUpdateFrame() {
		m_bNewDataAvailable = TRUE;
		m_dataAvailableEvent.Raise();
	}

	// Generator methods
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

		// fprintf(stderr, "X");
		const NUI_IMAGE_FRAME *pFrame = m_pReader->LockFrame();
		if (pFrame) {
			KINECT_LOCKED_RECT lockedRect;
			pFrame->pFrameTexture->LockRect(0, &lockedRect, NULL, 0);

			// FIXME check status code
			UpdateImageData(pFrame, (SourcePixelType*)lockedRect.pBits, lockedRect);

			//if (m_pReader->GetMirrorFactor() < 0) {
			//	// slow but works
			//	for (int y = 0; y < Y_RES; y++) {
			//		TargetPixelType* p = m_pBuffer + y * X_RES;
			//		for (int x = 0; x < X_RES / 2; x++) {
			//			TargetPixelType a = *(p + x);
			//			*(p + x) = *(p + X_RES - 1 - x);
			//			*(p + X_RES - 1 - x) = a;
			//		}
			//	}
			//}

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
			if (!m_pBuffer) {
				// lazily initialize the buffer because the size is unknown at Init
				m_pBuffer = new TargetPixelType[GetXRes() * GetYRes()];
				if (m_pBuffer == NULL) {
					return XN_STATUS_ALLOC_FAILED;
				}
				xnOSMemSet(m_pBuffer, 0, GetXRes() * GetYRes() * sizeof(TargetPixelType));
			}

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

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const SourcePixelType* data, const KINECT_LOCKED_RECT& lockedRect) = 0;

};
