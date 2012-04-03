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

	
};
