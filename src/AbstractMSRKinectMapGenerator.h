#pragma once
#include "base.h"
#include "MSRKinectManager.h"
#include <XnModuleCppInterface.h>
#include <XnEvent.h>

template <class ParentModuleMapGeneratorClass, class SourcePixelType, class TargetPixelType, NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION resolution>
class AbstractMSRKinectMapGenerator : public virtual ParentModuleMapGeneratorClass, public virtual xn::ModuleMirrorInterface
{
protected:
	AbstractMSRKinectMapGenerator() :
		m_isNuiInitialized(FALSE),
		m_bGenerating(FALSE), m_bMirror(FALSE),
		m_hNextFrameEvent(NULL), m_hStreamHandle(NULL), m_pFrame(NULL),
		m_pBuffer(NULL), m_nFrameID(0), m_lTimestamp(0)
	{
		InitializeCriticalSection(&m_csModifyFrame);
	}

public:
	virtual ~AbstractMSRKinectMapGenerator()
	{
		if (m_pBuffer) {
			delete[] m_pBuffer;
		}

		if (m_isNuiInitialized) {
			MSRKinectManager::getInstance()->shutdown();
		}

		if (m_hNextFrameEvent) {
			CloseHandle(m_hNextFrameEvent);
		}

		DeleteCriticalSection(&m_csModifyFrame);
	}

	XnStatus Init()
	{
		if (FAILED(MSRKinectManager::getInstance()->init())) {
			return XN_STATUS_INVALID_OPERATION;
		}
		m_isNuiInitialized = TRUE;

		m_hNextFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hNextFrameEvent == NULL) {
			return XN_STATUS_INVALID_OPERATION;
		}

		if (FAILED(NuiImageStreamOpen(eImageType, resolution, 0, 2, m_hNextFrameEvent, &m_hStreamHandle))) {
			return XN_STATUS_INVALID_OPERATION;
		}

		m_pBuffer = new TargetPixelType[X_RES * Y_RES];
		if (m_pBuffer == NULL) {
			return XN_STATUS_ALLOC_FAILED;
		}
		xnOSMemSet(m_pBuffer, 0, X_RES * Y_RES * sizeof(TargetPixelType));

		return XN_STATUS_OK;
	}

	// ProductionNode methods
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName)
	{
		return strcmp(strCapabilityName, XN_CAPABILITY_MIRROR) == 0;
	}

	// Generator methods
	virtual XnStatus StartGenerating()
	{
		XnStatus nRetVal = XN_STATUS_OK;
	
		m_bGenerating = TRUE;

		// start scheduler thread
		nRetVal = xnOSCreateThread(schedulerThread, this, &m_hScheduler);
		if (nRetVal != XN_STATUS_OK) {
			m_bGenerating = FALSE;
			return nRetVal;
		}

		m_generatingEvent.Raise();

		return XN_STATUS_OK;
	}

	static XN_THREAD_PROC schedulerThread( void* pCookie )
	{
		AbstractMSRKinectMapGenerator* that = (AbstractMSRKinectMapGenerator*)pCookie;

		while (that->m_bGenerating) {
			DWORD rc = WaitForSingleObject(that->m_hNextFrameEvent, INFINITE);
			if (rc == WAIT_OBJECT_0 && that->m_bGenerating) {
				// fprintf(stderr, ".");
				HRESULT hr = that->getFrame();
				ResetEvent(that->m_hNextFrameEvent);
				if (SUCCEEDED(hr)) {
					that->m_dataAvailableEvent.Raise();
				}
			}
		}

		XN_THREAD_PROC_RETURN(0);
	}

	virtual XnBool IsGenerating()
	{
		return m_bGenerating;
	}

	virtual void StopGenerating()
	{
		m_bGenerating = FALSE;

		// awake thread by fake event
		SetEvent(m_hNextFrameEvent);
		// wait for thread to exit
		xnOSWaitForThreadExit(m_hScheduler, 100);

		m_generatingEvent.Raise();
	}

	virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return m_generatingEvent.Register(handler, pCookie, &hCallback);
	}

	virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
	{
		m_generatingEvent.Unregister(hCallback);
	}

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
		return m_pFrame != NULL;
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
		// fprintf(stderr, "X");

		EnterCriticalSection(&m_csModifyFrame);
		const NUI_IMAGE_FRAME *pFrame = m_pFrame;
		if (pFrame) {
			KINECT_LOCKED_RECT lockedRect;
			pFrame->pFrameTexture->LockRect(0, &lockedRect, NULL, 0);

			// todo check status code
			UpdateImageData(pFrame, (SourcePixelType*)lockedRect.pBits, lockedRect);

			if (m_bMirror) {
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

			m_nFrameID++;
			m_lTimestamp = pFrame->liTimeStamp.QuadPart;
			clearFrame();
		} else {
			// keep the previous result
		}
		LeaveCriticalSection(&m_csModifyFrame);

		return XN_STATUS_OK;
	}

	// MapGenerator methods

	virtual XnUInt32 GetSupportedMapOutputModesCount()
	{
		return 1; // 640x480x30fps only
	}

	virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
	{
		if (nCount < 1) {
			return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
		}
		getSupportedMapOutputMode(&aModes[0]);
		return XN_STATUS_OK;
	}

	void getSupportedMapOutputMode(XnMapOutputMode* pMode)
	{
		pMode->nXRes = X_RES;
		pMode->nYRes = Y_RES;
		pMode->nFPS = FPS;
	}

	virtual XnStatus SetMapOutputMode(const XnMapOutputMode& mode)
	{
		XnMapOutputMode supportedMode;
		getSupportedMapOutputMode(&supportedMode);
		if (supportedMode.nXRes == mode.nXRes && supportedMode.nYRes == mode.nYRes && supportedMode.nFPS == mode.nFPS) {
			return XN_STATUS_OK;
		} else {
			return XN_STATUS_BAD_PARAM;
		}
	}

	virtual XnStatus GetMapOutputMode(XnMapOutputMode& mode)
	{
		getSupportedMapOutputMode(&mode);
		return XN_STATUS_OK;
	}

	virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		// ignore, maybe someday
		return XN_STATUS_OK;
	}

	virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
	{
		// ignore, maybe someday
	}

	virtual XnUInt64 GetTimestamp()
	{
		return m_lTimestamp;
	}

	virtual XnUInt32 GetFrameID()
	{
		return m_nFrameID;
	}

	virtual xn::ModuleMirrorInterface* GetMirrorInterface()
	{
		return this;
	}

	// Mirror methods
	virtual XnStatus SetMirror(XnBool bMirror)
	{
		m_bMirror = bMirror;
		m_mirrorEvent.Raise();
		return XN_STATUS_OK;
	}

	virtual XnBool IsMirrored()
	{
		return m_bMirror;
	}

	virtual XnStatus RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return m_mirrorEvent.Register(handler, pCookie, &hCallback);
	}

	virtual void UnregisterFromMirrorChange(XnCallbackHandle hCallback)
	{
		m_mirrorEvent.Unregister(hCallback);
	}

protected:
	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const SourcePixelType* data, const KINECT_LOCKED_RECT& lockedRect) = 0;

private:
	HRESULT getFrame()
	{
		EnterCriticalSection(&m_csModifyFrame);
		clearFrame();
		HRESULT hr = NuiImageStreamGetNextFrame(m_hStreamHandle, 100, &m_pFrame);
		LeaveCriticalSection(&m_csModifyFrame);
		return hr;
	}

	void clearFrame()
	{
		EnterCriticalSection(&m_csModifyFrame);
		if (m_pFrame) {
			NuiImageStreamReleaseFrame(m_hStreamHandle, m_pFrame);
			m_pFrame = NULL;
		}
		LeaveCriticalSection(&m_csModifyFrame);
	}

private:
	XN_THREAD_HANDLE m_hScheduler;

	BOOL m_bGenerating;
	BOOL m_bMirror;

	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);
	ChangeEvent m_generatingEvent;
	ChangeEvent m_dataAvailableEvent;
	ChangeEvent m_mirrorEvent;

	CRITICAL_SECTION m_csModifyFrame;
	BOOL m_isNuiInitialized;

protected:
	HANDLE m_hNextFrameEvent;
	HANDLE m_hStreamHandle;
	const NUI_IMAGE_FRAME *m_pFrame;

	TargetPixelType* m_pBuffer;

	XnUInt32 m_nFrameID;
	XnUInt64 m_lTimestamp;

	static const XnUInt32 X_RES = 640;
	static const XnUInt32 Y_RES = 480;
	static const XnUInt32 FPS = 30;
};
