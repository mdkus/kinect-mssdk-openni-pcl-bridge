#pragma once
#include "base.h"
#include "MSRKinectImageStreamManager.h"
#include <map>

class MSRKinectManager
{
private:
	typedef std::map<NUI_IMAGE_TYPE, MSRKinectImageStreamManager*> MSRKinectImageStreamManagerMap;

private:
	static MSRKinectManager* cs_pInstance;
	MSRKinectImageStreamManagerMap m_streamMap;

public:
	static MSRKinectManager* getInstance()
	{
		if (!cs_pInstance) {
			cs_pInstance = new MSRKinectManager();
			if (FAILED(cs_pInstance->Init())) {
				delete cs_pInstance;
				cs_pInstance = NULL;
			}
		}

		return cs_pInstance;
	}

	MSRKinectManager()
	{
	}

	virtual ~MSRKinectManager()
	{
		Shutdown();
	}

	HRESULT Init()
	{
		const DWORD nuiInitFlags =
			NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
			NUI_INITIALIZE_FLAG_USES_SKELETON |
			NUI_INITIALIZE_FLAG_USES_COLOR;

		try {
			CHECK_HRESULT(NuiInitialize(nuiInitFlags));
			CHECK_HRESULT(NuiSkeletonTrackingEnable(CreateEvent(NULL, TRUE, FALSE, NULL), 0));
			return S_OK;
		} catch (Win32ResultException& e) {
			// FIXME error log
			return e.hResult;
		}
	}

	MSRKinectImageStreamManager* GetImageStreamByType(NUI_IMAGE_TYPE eImageType)
	{

		switch (eImageType) {
		case NUI_IMAGE_TYPE_DEPTH:
		case NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX:
			return GetDepthStreamByTypeAndResolution(eImageType, NUI_IMAGE_RESOLUTION_320x240);
		case NUI_IMAGE_TYPE_COLOR:
			return GetDepthStreamByTypeAndResolution(eImageType, NUI_IMAGE_RESOLUTION_640x480);
		default:
			// FIXME error log or exception
			return NULL;
		}
	}

	void Shutdown()
	{
		for (MSRKinectImageStreamManagerMap::iterator i = m_streamMap.begin(); i != m_streamMap.end(); i++) {
			delete i->second;
		}
		m_streamMap.clear();

		NuiShutdown();
	}

private:
	MSRKinectImageStreamManager* GetDepthStreamByTypeAndResolution(NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution)
	{
		MSRKinectImageStreamManager* pStream = m_streamMap[eImageType];

		if (!pStream) {
			pStream = new MSRKinectImageStreamManager(eImageType, eResolution);
			m_streamMap[eImageType] = pStream;
		}

		return pStream;
	}

};
