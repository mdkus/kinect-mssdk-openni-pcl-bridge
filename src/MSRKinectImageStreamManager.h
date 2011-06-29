#pragma once
#include "base.h"
#include "MSRKinectImageStreamReader.h"

class MSRKinectImageStreamManager
{
private:
	HANDLE m_hNextFrameEvent;
	HANDLE m_hStreamHandle;
	MSRKinectImageStreamReader* m_pReader;

public:
	MSRKinectImageStreamReader* GetReader() { return m_pReader; }

public:
	MSRKinectImageStreamManager(NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution) :
		m_hNextFrameEvent(NULL), m_hStreamHandle(NULL), m_pReader(NULL)
		// throws XnStatusException
	{
		try {
			CHECK_XN_STATUS(xnOSCreateEvent(&m_hNextFrameEvent, TRUE));

			m_pReader = new MSRKinectImageStreamReader();
			m_pReader->Init(m_hNextFrameEvent, eImageType, eResolution);

		} catch (XnStatusException&) {
			if (m_pReader) delete m_pReader;
			if (m_hNextFrameEvent) xnOSCloseEvent(&m_hNextFrameEvent);
			throw;
		}
	}

	virtual ~MSRKinectImageStreamManager()
	{
		if (m_pReader) delete m_pReader;
		if (m_hNextFrameEvent) xnOSCloseEvent(&m_hNextFrameEvent);
	}

	void AddListener(MSRKinectImageStreamReader::Listener* listener)
	{
		m_pReader->AddListener(listener);
	}

	void RemoveListener(MSRKinectImageStreamReader::Listener* listener)
	{
		m_pReader->RemoveListener(listener);
	}
};
