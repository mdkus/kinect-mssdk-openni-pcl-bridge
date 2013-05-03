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
// Contributor: Michael Dingerkus <mdkus@web.de>,  Copyright (c) 2013
//@COPYRIGHT@//

#pragma once
#include "base.h"
#include "Properties.h"

// IntProperty for depth node: Set the output format
#define PROP_DEPTH_OUTPUT_FORMAT "OutputFormat"

#define DEFINE_BINARY_PROPERTY(name, size, data) \
	static const BYTE s_##name##Data[size+1] = data; \
	SetGeneralProperty(#name, size, s_##name##Data);

class PCLProperties : public Properties
{
public:
	typedef enum
    {
		OpenNI_shift_values = 0, // Shift values (disparity)
        OpenNI_12_bit_depth = 1, // Default mode: regular 12-bit depth
    };

	PCLProperties()
	{
		SetIntProperty(PROP_DEPTH_OUTPUT_FORMAT, OpenNI_12_bit_depth);
	}
};
