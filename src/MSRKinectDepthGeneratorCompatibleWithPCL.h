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
// Contributor: Michael Dingerkus <mdkus@web.de>, Copyright (c) 2013
//@COPYRIGHT@//

#pragma once
#include "base.h"
#include "MSRKinectDepthGeneratorCompatibleWithPrimeSense.h"
#include "PCLProperties.h"

// experimental
class MSRKinectDepthGeneratorCompatibleWithPCL :
	public MSRKinectDepthGeneratorCompatibleWithPrimeSense
{
private:
	typedef MSRKinectDepthGeneratorCompatibleWithPrimeSense SuperClass;

private:
	PCLProperties m_properties;

public:
	XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
	{
		// sorry for using downcast!
		if (streq(strName, PROP_DEPTH_OUTPUT_FORMAT)) {
			if ( ((MSRKinectDepthImageStreamReader*) m_pReader)->GetImageType() == NUI_IMAGE_TYPE_DEPTH ) 
				nValue = PCLProperties::OpenNI_12_bit_depth;
			else
				nValue = PCLProperties::OpenNI_shift_values;				
			return XN_STATUS_OK;
		} else {
			return SuperClass::GetIntProperty(strName, nValue);
		}
	}

	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
	{
		// sorry for using downcast!
		if (streq(strName, PROP_DEPTH_OUTPUT_FORMAT)) {
			MSRKinectRequirement* pRequirement = ((MSRKinectDepthImageStreamReader*) m_pReader)->GetRequirement();
			DWORD initFlags = pRequirement->GetInitFlags();
			if ( nValue == PCLProperties::OpenNI_shift_values )
			{				
				if ( initFlags & NUI_INITIALIZE_FLAG_USES_DEPTH )
					pRequirement->SwitchToDepthRequirement(XN_NODE_TYPE_USER);
				else if ( !(initFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX) )
				{
					pRequirement->AddRequirement(XN_NODE_TYPE_DEPTH, m_imageConfig.GetSelectedMode()->outputMode);
					pRequirement->SwitchToDepthRequirement(XN_NODE_TYPE_USER);
					/// /todo reset camera?
				}
			}
			else if ( nValue == PCLProperties::OpenNI_12_bit_depth )
			{
				if( !(initFlags & NUI_INITIALIZE_FLAG_USES_DEPTH ) )
				{
					if ( initFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX )
						pRequirement->SwitchToDepthRequirement(XN_NODE_TYPE_DEPTH);
						/// /todo reset camera?
					else
						pRequirement->AddRequirement(XN_NODE_TYPE_DEPTH, m_imageConfig.GetSelectedMode()->outputMode);
				}
			}
			return XN_STATUS_OK;
		} else {
			return SuperClass::SetIntProperty(strName, nValue);
		}
	}

};
