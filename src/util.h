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
#include <string>

template<typename T>
inline T changeBit(T value, T mask, BOOL toSet)
{
	return toSet ? (value | mask) : (value & ~mask);
}

inline int streq(const char* s1, const char* s2)
{
	return strcmp(s1, s2) == 0;
}

// BSTR sucks
inline std::string bstr2cstr(BSTR s)
{
	int len = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
	XnChar* buf = new XnChar[len];
	WideCharToMultiByte(CP_ACP, 0, s, -1, buf, len, NULL, NULL);
	return buf;
}

inline BSTR cstr2bstr(const char* s)
{
	int len = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0) - 1;
	BSTR b = SysAllocStringByteLen(NULL, len * sizeof(OLECHAR));
	MultiByteToWideChar(CP_ACP, 0, s, -1, b, len);
	return b;
}