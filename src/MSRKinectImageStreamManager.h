#pragma once
#include "base.h"
#include "MSRKinectImageStreamReader.h"
#include "MSRKinectRequirement.h"

class MSRKinectImageStreamManager
{
private:
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
	}

	void Init(MSRKinectRequirement* pRequirement)
	{
		try {
			CHECK_XN_STATUS(xnOSCreateEvent(&m_hNextFrameEvent, TRUE));

			m_pReader = CreateReader();
			m_pReader->Init(pRequirement, m_hNextFrameEvent);

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

protected:
	virtual MSRKinectImageStreamReader* CreateReader() = 0;
};

class MSRKinectColorImageStreamManager : public MSRKinectImageStreamManager
{
	virtual MSRKinectImageStreamReader* CreateReader()
	{
		return new MSRKinectColorImageStreamReader();
	}
};

class MSRKinectDepthImageStreamManager : public MSRKinectImageStreamManager
{
	virtual MSRKinectImageStreamReader* CreateReader()
	{
		return new MSRKinectDepthImageStreamReader();
	}
};
