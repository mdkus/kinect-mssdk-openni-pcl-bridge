#pragma once
#include "base.h"
#include "MSRKinectManager.h"
#include "MSRKinectImageStreamReader.h"
#include "MSRKinectMirrorCap.h"

template <class ParentModuleGeneratorClass, class SourcePixelType, class TargetPixelType, NUI_IMAGE_TYPE eImageType>
class AbstractMSRKinectFrameGenerator :
	public virtual ParentModuleGeneratorClass,
	public virtual MSRKinectImageStreamReader::Listener
{
private:
	typedef AbstractMSRKinectFrameGenerator<ParentModuleGeneratorClass, SourcePixelType, TargetPixelType, eImageType> ThisClass;
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

private:
	ChangeEvent m_dataAvailableEvent;

protected:
	MSRKinectImageStreamReader* m_pReader;
	BOOL m_bIsNewDataAvailable;
	TargetPixelType* m_pBuffer;

	static const XnUInt32 X_RES = 640;
	static const XnUInt32 Y_RES = 480;
	static const XnUInt32 FPS = 30;

protected:
	AbstractMSRKinectFrameGenerator() :
		m_pBuffer(NULL),
		m_bIsNewDataAvailable(FALSE)
	{
	}

public:
	virtual ~AbstractMSRKinectFrameGenerator()
	{
		if (m_pBuffer) {
			delete[] m_pBuffer;
		}
	}

	virtual XnStatus Init()
	{
		HANDLE hNextFrameEvent = NULL;
		try {
			MSRKinectManager* pMan = MSRKinectManager::getInstance();

			m_pReader = pMan->GetImageStreamManagerByType(eImageType)->GetReader();
			m_pReader->AddListener(this);

			m_pBuffer = new TargetPixelType[X_RES * Y_RES];
			if (m_pBuffer == NULL) {
				return XN_STATUS_ALLOC_FAILED;
			}
			xnOSMemSet(m_pBuffer, 0, X_RES * Y_RES * sizeof(TargetPixelType));

			return XN_STATUS_OK;
		} catch (XnStatusException& e) {
			if (hNextFrameEvent) xnOSCloseEvent(&hNextFrameEvent);
			return e.nStatus;
		}
	}

	virtual void OnUpdateFrame() {
		m_bIsNewDataAvailable = TRUE;
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
		return m_bIsNewDataAvailable;
	}

	virtual const void* GetData()
	{
		return m_pBuffer;
	}

	virtual XnUInt32 GetDataSize()
	{
		return X_RES * Y_RES * sizeof(TargetPixelType);
	}

	virtual XnStatus UpdateData()
	{
		if (!m_bIsNewDataAvailable) {
			return XN_STATUS_OK;
		}

		// fprintf(stderr, "X");
		const NUI_IMAGE_FRAME *pFrame = m_pReader->LockFrame();
		if (pFrame) {
			KINECT_LOCKED_RECT lockedRect;
			pFrame->pFrameTexture->LockRect(0, &lockedRect, NULL, 0);

			// FIXME check status code
			UpdateImageData(pFrame, (SourcePixelType*)lockedRect.pBits, lockedRect);

			if (m_pReader->IsMirror()) {
				// slow but works
				for (int y = 0; y < Y_RES; y++) {
					TargetPixelType* p = m_pBuffer + y * X_RES;
					for (int x = 0; x < X_RES / 2; x++) {
						TargetPixelType a = *(p + x);
						*(p + x) = *(p + X_RES - 1 - x);
						*(p + X_RES - 1 - x) = a;
					}
				}
			}

			m_bIsNewDataAvailable = FALSE;
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

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const SourcePixelType* data, const KINECT_LOCKED_RECT& lockedRect) = 0;

};
