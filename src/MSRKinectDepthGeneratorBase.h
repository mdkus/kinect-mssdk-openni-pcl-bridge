//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#pragma once
#include "base.h"
#include "AbstractMSRKinectMapGenerator.h"

template<class ParentMapGeneratorClass, class DepthPixelProcessor>
class MSRKinectDepthGeneratorBase :
	public AbstractMSRKinectMapGenerator<ParentMapGeneratorClass, USHORT, XnDepthPixel>
{
private:
	static const ImageConfiguration::Desc* GetImageConfigDesc()
	{
		static ImageConfiguration::Mode s_modes[] = {
			ImageConfiguration::Mode(640, 480, 30),
			ImageConfiguration::Mode(320, 240, 30)
		};
		static ImageConfiguration::Desc s_desc(s_modes, 2);
		return &s_desc;
	}

protected:
	MSRKinectDepthGeneratorBase(XnPredefinedProductionNodeType nodeType, BOOL bActiveGeneratorControl) :
		 AbstractMSRKinectMapGenerator(nodeType, bActiveGeneratorControl, GetImageConfigDesc())
	{
	}

public:
	virtual ~MSRKinectDepthGeneratorBase() {}

protected:
	XnStatus UpdateDepthData(DepthPixelProcessor& proc, const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect)
	{
		const USHORT* sp = data;
		XnDepthPixel* dp = m_pBuffer;

		XnUInt32 destXRes = GetXRes();
		XnUInt32 destYRes = GetYRes();
		DWORD sourceXRes;
		DWORD sourceYRes;
		NuiImageResolutionToSize(pFrame->eResolution, sourceXRes, sourceYRes);
		int ratio = destXRes / sourceXRes;
		assert(ratio != 0);
		int step = m_pReader->GetMirrorFactor();

		BOOL hasPlayerIndex = m_pReader->GetImageType() == NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX;

		if (!m_pReader->IsCalibrateViewPoint()) {
			for (XnUInt32 y = 0; y < sourceYRes; y++) {
				sp = data + y * sourceXRes + (step < 0 ? sourceXRes-1 : 0);
				for (XnUInt32 x = 0; x < sourceXRes; x++) {
#if KINECTSDK_VER >= 100
					USHORT d = (*sp);
#else
					USHORT d = hasPlayerIndex ? (*sp) : (*sp << 3);
#endif
					proc.Process(d, dp, ratio);
					sp += step;
					dp += ratio;
				}
				dp += destXRes * (ratio-1);
			}
		} else {
			memset(m_pBuffer, 0, destXRes * destYRes);

#if KINECTSDK_VER >= 100
			const XnUInt32 pixelSize = max(1, destXRes / sourceXRes);

			/*for (XnUInt32 y = 0; y < sourceYRes; y++) {
				sp = data + y * sourceXRes + (step < 0 ? sourceXRes-1 : 0);
				for (XnUInt32 x = 0; x < sourceXRes; x++) {
					LONG ix, iy;
					USHORT d = *sp;
					NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
						(destXRes == 640 ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240), pFrame->eResolution, NULL,
						x, y, (d & ~NUI_IMAGE_PLAYER_INDEX_MASK), &ix, &iy);
					if (ix >= 0 && ix <= LONG(destXRes - pixelSize) && iy >= 0 && iy <= LONG(destYRes - pixelSize)) {
						proc.Process(d, m_pBuffer + iy * destXRes + ix, pixelSize);
					}
					sp += step;
				}
			}*/

			//OutpostStudios: Henrik Weirauch
			MSRKinectManager * KM = MSRKinectManager::GetInstance();
			
				USHORT * pData = (USHORT*)data;

				DWORD cColorCoordinates  = destXRes*destYRes*2;
				LONG * pColorCoordinates = new LONG[cColorCoordinates];
				const LONG * pCC;
				
				HRESULT hr = KM->GetRequirement()->GetSensor()->
						NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution
						(
							(destXRes == 640 ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240)
							, pFrame->eResolution/*source*/
							, sourceXRes*sourceYRes, pData
							, cColorCoordinates, pColorCoordinates 
						);

				if(FAILED(hr)){
					printf("colorres: %d\n",(destXRes == 640 ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240));
					printf("depthres: %d\n",pFrame->eResolution);
					printf("sourcebyte: %d ptr %d\n", sourceXRes*sourceYRes,  pData);
					printf("destbyte: %d ptr %d\n", cColorCoordinates,  pColorCoordinates);
					CHECK_HRESULT(hr);
				}
				for (XnUInt32 y = 0; y < sourceYRes; y++) 
				{
					sp = data + y * sourceXRes + (step < 0 ? sourceXRes-1 : 0);
					pCC= pColorCoordinates + y * pixelSize * destXRes*2 + (step < 0 ? (destXRes*2)-2 : 0);

					for (XnUInt32 x = 0; x < sourceXRes; x++)
					{
						LONG ix = (step < 0 ? destXRes-1-pCC[0] : pCC[0]); 
						LONG iy = pCC[1];
						USHORT d = *sp;

						if (ix >= 0 && ix <= LONG(destXRes - pixelSize) && iy >= 0 && iy <= LONG(destYRes - pixelSize)) 
						{
							proc.Process(d, m_pBuffer + iy * destXRes + ix, pixelSize );
						}
						sp += step;
						pCC+= (step*2*pixelSize);
					}
				}

				delete [] pColorCoordinates;

			KM->Release();

#else
			// NuiImageGetColorPixelCoordinatesFromDepthPixel only supports Image@640x480 and Depth@320x240
			const XnUInt32 convXRes = 320;
			const XnUInt32 convYRes = 240;
			const XnUInt32 convRatio = sourceXRes / convXRes;
			assert(convRatio != 0);
			const XnUInt32 destRatio = destXRes / convXRes;
			assert(destRatio != 0);

			for (XnUInt32 y = 0; y < convYRes; y++) {
				sp = data + y * sourceXRes * convRatio + (step < 0 ? sourceXRes-1 : 0);
				for (XnUInt32 x = 0; x < convXRes; x++) {
					LONG ix, iy;
					USHORT d = hasPlayerIndex ? (*sp) : (*sp << 3);
					NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, (d & ~NUI_IMAGE_PLAYER_INDEX_MASK), &ix, &iy);
					if (ix >= 0 && ix <= LONG(640-2) && iy >= 0 && iy <= LONG(480-2)) {
						if (destRatio == 1) {
							ix >>= 1;
							iy >>= 1;
						}
						proc.Process(d, m_pBuffer + iy * convXRes * destRatio + ix, destRatio);
					}
					sp += convRatio * step;
				}
			}
#endif
		}

		return XN_STATUS_OK;
	}

	virtual XnStatus UpdateImageData(const NUI_IMAGE_FRAME* pFrame, const USHORT* data, const NUI_LOCKED_RECT& lockedRect) = 0;
};
