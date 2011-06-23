#pragma once
#include "base.h"

class MSRKinectManager
{
private:
	static MSRKinectManager cs_instance;

	int initCount;

public:
	static MSRKinectManager* getInstance()
	{
		return &cs_instance;
	}

	MSRKinectManager() : initCount(0)
	{
	}

	HRESULT init()
	{
		if (0 == initCount++) {
			return NuiInitialize(
				NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
				NUI_INITIALIZE_FLAG_USES_SKELETON |
				NUI_INITIALIZE_FLAG_USES_COLOR);
		} else {
			return S_OK;
		}
	}

	void shutdown()
	{
		if (--initCount == 0) {
			NuiShutdown();
		}
	}

};
