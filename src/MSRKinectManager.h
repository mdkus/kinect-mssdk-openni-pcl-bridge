#pragma once
#include "base.h"
#include "MSRKinectImageStreamManager.h"
#include "MSRKinectSkeletonManager.h"
#include <map>

class MSRKinectManager
{
private:
	typedef std::map<NUI_IMAGE_TYPE, MSRKinectImageStreamManager*> MSRKinectImageStreamManagerMap;

private:
	static MSRKinectManager* cs_pInstance;

	MSRKinectImageStreamManagerMap m_streamManagerMap;
	MSRKinectSkeletonManager* m_pSkeletonManager;

public:
	static MSRKinectManager* getInstance() // throws XnStatusException
	{
		if (!cs_pInstance) {
			cs_pInstance = new MSRKinectManager();
		}

		return cs_pInstance;
	}

	MSRKinectManager() : m_pSkeletonManager(NULL) // throws XnStatusException
	{
		const DWORD nuiInitFlags =
			NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
			NUI_INITIALIZE_FLAG_USES_SKELETON |
			NUI_INITIALIZE_FLAG_USES_COLOR;

		CHECK_HRESULT(NuiInitialize(nuiInitFlags));
	}

	virtual ~MSRKinectManager()
	{
		Shutdown();
	}

	MSRKinectImageStreamManager* GetImageStreamManagerByType(NUI_IMAGE_TYPE eImageType) // throws XnStatusException
	{

		switch (eImageType) {
		case NUI_IMAGE_TYPE_DEPTH:
		case NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX:
			return GetDepthStreamByTypeAndResolution(eImageType, NUI_IMAGE_RESOLUTION_320x240);
		case NUI_IMAGE_TYPE_COLOR:
			return GetDepthStreamByTypeAndResolution(eImageType, NUI_IMAGE_RESOLUTION_640x480);
		default:
			// FIXME error log
			throw XnStatusException(XN_STATUS_BAD_PARAM);
		}
	}

	MSRKinectSkeletonManager* GetSkeletonManager() // throws XnStatusException
	{
		if (!m_pSkeletonManager) {
			m_pSkeletonManager = new MSRKinectSkeletonManager();
		}
		return m_pSkeletonManager;
	}

	void Shutdown()
	{
		for (MSRKinectImageStreamManagerMap::iterator i = m_streamManagerMap.begin(); i != m_streamManagerMap.end(); i++) {
			delete i->second;
		}
		m_streamManagerMap.clear();

		if (m_pSkeletonManager) {
			delete m_pSkeletonManager;
		}

		NuiShutdown();
	}

private:
	MSRKinectImageStreamManager* GetDepthStreamByTypeAndResolution(NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution)
	{
		MSRKinectImageStreamManager* pStream = m_streamManagerMap[eImageType];

		if (!pStream) {
			pStream = new MSRKinectImageStreamManager(eImageType, eResolution);
			m_streamManagerMap[eImageType] = pStream;
		}

		return pStream;
	}

};
