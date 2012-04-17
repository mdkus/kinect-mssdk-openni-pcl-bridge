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
#include <Windows.h>

#if KINECTSDK_VER >= 100
#include <NuiApi.h>
#else
#include <MSR_NuiApi.h>
typedef KINECT_LOCKED_RECT NUI_LOCKED_RECT;
#endif

#include <XnCppWrapper.h>
#include <XnModuleCppInterface.h>
#include <XnEvent.h>
#include <XnLog.h>

#pragma warning(disable : 4996)

struct XnStatusException {
	const XnStatus nStatus;
	const HRESULT hResult;

	XnStatusException(XnStatus n, HRESULT hr = S_OK) : nStatus(n), hResult(hr) {}
};

#define CHECK_XN_STATUS(statement) \
{ \
	XnStatus __status = (statement); \
	if (__status != XN_STATUS_OK) { xnPrintError(__status, #statement); throw XnStatusException(__status); } \
}

inline void printHResult(HRESULT hr, const char* statement)
{
	LPTSTR message;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&message, 0, NULL);
	fprintf(stderr, "%s (HRESULT=%08x): %s\n", message, hr, statement);
	LocalFree(message);
}

#define CHECK_HRESULT(statement) \
{ \
	HRESULT __hr = (statement); \
	if (FAILED(__hr)) { printHResult(__hr, #statement); throw XnStatusException(XN_STATUS_ERROR, __hr); } \
}

#define LOG(format, ...) fprintf(stderr, format, __VA_ARGS__)

