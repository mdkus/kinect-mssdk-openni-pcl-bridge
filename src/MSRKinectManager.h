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
#include "MSRKinectImageStreamManager.h"
#include "MSRKinectSkeletonManager.h"
#include "MSRKinectAudioStreamManager.h"
#include "MSRKinectRequirement.h"

class MSRKinectManager
{
private:
	static MSRKinectManager* cs_pInstance;

	int m_refCount;

	MSRKinectRequirement m_requirement;

	MSRKinectImageStreamManager* m_pColorImageStreamManager;
	MSRKinectImageStreamManager* m_pDepthImageStreamManager;
	MSRKinectSkeletonManager* m_pSkeletonManager;
	MSRKinectAudioStreamManager* m_pAudioStreamManager;

public:
	// Note: call Release() once per call of GetInstance()
	static MSRKinectManager* GetInstance() // throws XnStatusException
	{
		if (cs_pInstance == NULL) {
			cs_pInstance = new MSRKinectManager();
		}

		cs_pInstance->m_refCount++;

		return cs_pInstance;
	}

	MSRKinectManager() : m_refCount(0), m_pColorImageStreamManager(NULL), m_pDepthImageStreamManager(NULL), m_pSkeletonManager(NULL), m_pAudioStreamManager(NULL)
	{
	}

	virtual ~MSRKinectManager()
	{
		Shutdown();
	}

	void Release() {
		if (--m_refCount == 0) {
			delete this;
			cs_pInstance = NULL;
		}
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

	MSRKinectAudioStreamManager* GetAudioStreamManager() // throws XnStatusException
	{
		if (!m_pAudioStreamManager) {
			m_pAudioStreamManager = new MSRKinectAudioStreamManager(&m_requirement);
		}
		return m_pAudioStreamManager;
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
		m_requirement.DoShutdown();
	}

};
