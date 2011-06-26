#pragma once;
#include "base.h"
#include "MSRKinectFrameReader.h"
#include "MSRKinectSkeletonContext.h"

class MSRKinectSkeletonReader : public MSRKinectFrameReader<MSRKinectSkeletonContext>
{
public:
	void Init(HANDLE hNextFrameEvent)
	{
		InitContext(hNextFrameEvent);
	}
};
