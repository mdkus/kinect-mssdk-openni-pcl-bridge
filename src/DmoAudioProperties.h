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

#include "Prop.h"
#include <wmcodecdsp.h>

class DmoAudioProperties
{
private:
	IPropertyStore* m_pStore;

	int m_agc;
	int m_bam;
	int m_aec;
	int m_cc;
	int m_ns;

	Prop m_propSystemMode;
	Prop m_propFeatureMode;
	Prop m_propAES;
	Prop m_propNS;
	Prop m_propAGC;
	Prop m_propMicArrayMode;
	Prop m_propCenterClip;
	Prop m_propNoiseFill;

public:
	DmoAudioProperties() :
		m_pStore(NULL),
		m_agc(0), m_bam(0), m_aec(2), m_cc(0), m_ns(1), // default values
		m_propSystemMode(MFPKEY_WMAAECMA_SYSTEM_MODE),
		m_propFeatureMode(MFPKEY_WMAAECMA_FEATURE_MODE),
		m_propAES(MFPKEY_WMAAECMA_FEATR_AES),
		m_propNS(MFPKEY_WMAAECMA_FEATR_NS),
		m_propAGC(MFPKEY_WMAAECMA_FEATR_AGC),
		m_propMicArrayMode(MFPKEY_WMAAECMA_FEATR_MICARR_MODE),
		m_propCenterClip(MFPKEY_WMAAECMA_FEATR_CENTER_CLIP),
		m_propNoiseFill(MFPKEY_WMAAECMA_FEATR_NOISE_FILL)
	{
	}

	void SetPropertyStore(IPropertyStore* pStore)
	{
		m_pStore = pStore;
	}

	// 0: OFF (default)
	// 1: ON
	int GetAutomaticGainControl() { return m_agc; }
	void SetAutomaticGainControl(int value) { m_agc = value; Update(); }

	// 0: Adaptive (default)
	// 1: Automatic
	// 2: Manual
	int GetBeamAngleMode() { return m_bam; }
	void SetBeamAngleMode(int value) { m_bam = value; Update(); }

	// 0: None
	// 1: AEC
	// 2: AEC + AES x 1 (default)
	// 3: AEC + AEX x 2
	int GetEchoCancellationMode() { return m_aec; }
	void SetEchoCancellationMode(int value) { m_aec = value; Update(); }

	// 0: None
	// 1: Center Clip
	// 2: Center Clip + Noise Fill
	int GetCenterClippingMode() { return m_cc; }
	void SetCenterClippingMode(int value) { m_cc = value; Update(); }

	int GetNoiseSuppression() { return m_ns; }
	void SetNoiseSuppression(int value) { m_ns = value; Update(); }

	void Update() // throws XnStatusException
	{
		if (!m_pStore) return; // not initialized yet. defer.

		PrepareValuesForPush();

		// Feature mode must be always true
		m_propFeatureMode.var.SetBool(TRUE);
		m_propFeatureMode.PushIfChanged(m_pStore);

		m_propSystemMode.PushIfChanged(m_pStore);
		m_propAES.PushIfChanged(m_pStore);
		m_propNS.PushIfChanged(m_pStore);
		m_propAGC.PushIfChanged(m_pStore);
		m_propCenterClip.PushIfChanged(m_pStore);
		m_propNoiseFill.PushIfChanged(m_pStore);
		m_propMicArrayMode.PushIfChanged(m_pStore);
	}

private:
	void PrepareValuesForPush()
	{
		static const struct {
			AEC_SYSTEM_MODE systemMode;
			int aes;
		} AEC_MODE_MAP[] = {
			{ OPTIBEAM_ARRAY_ONLY,    0 },
			{ OPTIBEAM_ARRAY_AND_AEC, 0 },
			{ OPTIBEAM_ARRAY_AND_AEC, 1 },
			{ OPTIBEAM_ARRAY_AND_AEC, 2 },
		};

		if (m_aec >= 0 && m_aec <= 3) {
			m_propSystemMode.var.SetInt32(AEC_MODE_MAP[m_aec].systemMode);
			m_propAES.var.SetInt32(AEC_MODE_MAP[m_aec].aes);
		} else {
			throw XnStatusException(XN_STATUS_BAD_PARAM);
		}

		m_propNS.var.SetInt32(m_ns ? 1 : 0);
		m_propAGC.var.SetBool(m_agc);

		static const struct {
			BOOL centerClip;
			BOOL noiseFill;
		} CENTER_CLIPPING_MDOE_MAP[] = {
			{ FALSE, FALSE },
			{ TRUE,  FALSE },
			{ TRUE,  TRUE  },
		};

		if (m_cc >= 0 && m_cc < 2) {
			m_propCenterClip.var.SetBool(CENTER_CLIPPING_MDOE_MAP[m_cc].centerClip);
			m_propNoiseFill.var.SetBool(CENTER_CLIPPING_MDOE_MAP[m_cc].noiseFill);
		} else {
			throw XnStatusException(XN_STATUS_BAD_PARAM);
		}

		static const int BEAM_ANGLE_MODE_MAP[] = {
			MICARRAY_ADAPTIVE_BEAM,
			MICARRAY_SINGLE_BEAM,
			MICARRAY_EXTERN_BEAM,
		};

		if (m_bam >= 0 && m_bam <= 2) {
			m_propMicArrayMode.var.SetInt32(BEAM_ANGLE_MODE_MAP[m_bam]);
		}
	}
};
