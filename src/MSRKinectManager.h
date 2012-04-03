#pragma once
#include "base.h"
#include "MSRKinectImageStreamManager.h"
#include "MSRKinectSkeletonManager.h"
#include "MSRKinectRequirement.h"

class MSRKinectManager
{
private:
	static MSRKinectManager cs_instance;

	MSRKinectRequirement m_requirement;
	MSRKinectImageStreamManager* m_pColorImageStreamManager;
	MSRKinectImageStreamManager* m_pDepthImageStreamManager;
	MSRKinectSkeletonManager* m_pSkeletonManager;

public:
	static MSRKinectManager* GetInstance() // throws XnStatusException
	{
		return &cs_instance;
	}

	MSRKinectManager() : m_pColorImageStreamManager(NULL), m_pDepthImageStreamManager(NULL), m_pSkeletonManager(NULL)
	{
	}

	virtual ~MSRKinectManager()
	{
		Shutdown();
	}

	MSRKinectRequirement* GetRequirement()
	{
		return &m_requirement;
	}

	MSRKinectImageStreamManager* GetImageStreamManager(XnPredefinedProductionNodeType nodeType) // throws XnStatusException
	{
		switch (nodeType) {
		case XN_NODE_TYPE_IMAGE:
			if (!m_pColorImageStreamManager) {
				m_pColorImageStreamManager = new MSRKinectColorImageStreamManager(&m_requirement);
			}
			return m_pColorImageStreamManager;
		case XN_NODE_TYPE_DEPTH:
		case XN_NODE_TYPE_USER:
			if (!m_pDepthImageStreamManager) {
				m_pDepthImageStreamManager = new MSRKinectDepthImageStreamManager(&m_requirement);
			}
			return m_pDepthImageStreamManager;
		default:
			throw new XnStatusException(XN_STATUS_BAD_PARAM);
		}
	}

	MSRKinectSkeletonManager* GetSkeletonManager() // throws XnStatusException
	{
		if (!m_pSkeletonManager) {
			m_pSkeletonManager = new MSRKinectSkeletonManager(&m_requirement);
		}
		return m_pSkeletonManager;
	}

	void Shutdown()
	{
		if (m_pSkeletonManager) {
			delete m_pSkeletonManager;
		}
		if (m_pColorImageStreamManager) {
			delete m_pColorImageStreamManager;
		}
		if (m_pDepthImageStreamManager) {
			delete m_pDepthImageStreamManager;
		}

		NuiShutdown();
	}
};
