#pragma once;
#include "base.h"

#pragma once
#include "base.h"
#include "MSRKinectSkeletonReader.h"

class MSRKinectSkeletonManager
{
private:
	HANDLE m_hNextFrameEvent;
	MSRKinectSkeletonReader* m_pReader;

public:
	MSRKinectSkeletonReader* GetReader() { return m_pReader; }

public:
	MSRKinectSkeletonManager(MSRKinectRequirement* pRequirement) : m_hNextFrameEvent(NULL), m_pReader(NULL) // throws XnStatusException
	{
		try {
			CHECK_XN_STATUS(xnOSCreateEvent(&m_hNextFrameEvent, TRUE));
			m_pReader = new MSRKinectSkeletonReader(pRequirement, m_hNextFrameEvent);
		} catch (XnStatusException&) {
			if (m_pReader) delete m_pReader;
			if (m_hNextFrameEvent) xnOSCloseEvent(&m_hNextFrameEvent);
			throw;
		}
	}

	virtual ~MSRKinectSkeletonManager()
	{
		if (m_pReader) m_pReader->Stop();
		if (m_hNextFrameEvent) xnOSCloseEvent(&m_hNextFrameEvent);
	}

	void AddListener(MSRKinectSkeletonReader::Listener* listener)
	{
		m_pReader->AddListener(listener);
	}

	void RemoveListener(MSRKinectSkeletonReader::Listener* listener)
	{
		m_pReader->RemoveListener(listener);
	}
};
