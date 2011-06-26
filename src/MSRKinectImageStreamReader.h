#pragma once
#include "MSRKinectFrameReader.h"
#include "MSRKinectImageStreamContext.h"

class MSRKinectImageStreamReader : public MSRKinectFrameReader<MSRKinectImageStreamContext>
{
public:
	void Init(HANDLE hStreamHandle, HANDLE hNextFrameEvent)
	{
		InitContext(hStreamHandle, hNextFrameEvent);
	}
};
