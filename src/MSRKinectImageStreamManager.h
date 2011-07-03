#pragma once
#include "base.h"
#include "MSRKinectImageStreamReader.h"
#include "MSRKinectRequirement.h"

class MSRKinectImageStreamManager
{
protected:
	HANDLE m_hNextFrameEvent;
	HANDLE m_hStreamHandle;
	MSRKinectImageStreamReader* m_pReader;

public:
	MSRKinectImageStreamReader* GetReader() { return m_pReader; }

public:
	MSRKinectImageStreamManager() :
	  m_hNextFrameEvent(NULL), m_hStreamHandle(NULL), m_pReader(NULL)
		// throws XnStatusException
	{
		CHECK_XN_STATUS(xnOSCreateEvent(&m_hNextFrameEvent, TRUE));
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

class MSRKinectColorImageStreamManager : public MSRKinectImageStreamManager
{
public:
	MSRKinectColorImageStreamManager(MSRKinectRequirement* pRequirement)
	{
		m_pReader = new MSRKinectColorImageStreamReader(pRequirement, m_hNextFrameEvent);
	}
};

class MSRKinectDepthImageStreamManager : public MSRKinectImageStreamManager
{
public:
	MSRKinectDepthImageStreamManager(MSRKinectRequirement* pRequirement)
	{
		m_pReader = new MSRKinectDepthImageStreamReader(pRequirement, m_hNextFrameEvent);
	}
};
