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

#include "nui_error.h"
#include <NuiApi.h>

#define NUI_MSG(m) msg = m;

char* chomp(char* buf)
{
	for (int i = strlen(buf); i > 0 && (buf[i-1] == '\n' || buf[i-1] == '\r'); i--) {
		buf[i-1] = '\0';
	}
	return buf;
}

int getNuiErrorString(HRESULT hr, char* buf, int size)
{
	const char* msg = NULL;
	switch (hr) {
	case E_NUI_DEVICE_NOT_CONNECTED:	NUI_MSG("E_NUI_DEVICE_NOT_CONNECTED"); break;
	case E_NUI_DEVICE_NOT_READY:		NUI_MSG("E_NUI_DEVICE_NOT_READY"); break;
	case E_NUI_ALREADY_INITIALIZED:		NUI_MSG("E_NUI_ALREADY_INITIALIZED"); break;
	case E_NUI_NO_MORE_ITEMS:			NUI_MSG("E_NUI_NO_MORE_ITEMS"); break;
	case S_NUI_INITIALIZING:			NUI_MSG("S_NUI_INITIALIZING"); break;
	case E_NUI_FRAME_NO_DATA:			NUI_MSG("E_NUI_FRAME_NO_DATA"); break;
	case E_NUI_STREAM_NOT_ENABLED:		NUI_MSG("E_NUI_STREAM_NOT_ENABLED"); break;
	case E_NUI_IMAGE_STREAM_IN_USE:		NUI_MSG("E_NUI_IMAGE_STREAM_IN_USE"); break;
	case E_NUI_FRAME_LIMIT_EXCEEDED:	NUI_MSG("E_NUI_FRAME_LIMIT_EXCEEDED"); break;
	case E_NUI_FEATURE_NOT_INITIALIZED:	NUI_MSG("E_NUI_FEATURE_NOT_INITIALIZED"); break;
	case E_NUI_NOTGENUINE:				NUI_MSG("E_NUI_NOTGENUINE"); break;
	case E_NUI_INSUFFICIENTBANDWIDTH:	NUI_MSG("E_NUI_INSUFFICIENTBANDWIDTH"); break;
	case E_NUI_NOTSUPPORTED:			NUI_MSG("E_NUI_NOTSUPPORTED"); break;
	case E_NUI_DEVICE_IN_USE:			NUI_MSG("E_NUI_DEVICE_IN_USE"); break;
	case E_NUI_DATABASE_NOT_FOUND:		NUI_MSG("E_NUI_DATABASE_NOT_FOUND"); break;
	case E_NUI_DATABASE_VERSION_MISMATCH:		NUI_MSG("E_NUI_DATABASE_VERSION_MISMATCH"); break;
	case E_NUI_HARDWARE_FEATURE_UNAVAILABLE:	NUI_MSG("E_NUI_HARDWARE_FEATURE_UNAVAILABLE"); break;
	case E_NUI_NOTCONNECTED:			NUI_MSG("E_NUI_NOTCONNECTED"); break;
	case E_NUI_NOTREADY:				NUI_MSG("E_NUI_NOTREADY"); break;
	case E_NUI_SKELETAL_ENGINE_BUSY:	NUI_MSG("E_NUI_SKELETAL_ENGINE_BUSY"); break;
	case E_NUI_NOTPOWERED:				NUI_MSG("E_NUI_NOTPOWERED"); break;
	case E_NUI_BADIINDEX:				NUI_MSG("E_NUI_BADIINDEX"); break;
	}

	if (msg) {
		strncpy_s(buf, size, msg, _TRUNCATE);
	} else {
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, size, NULL);
	}
	buf[size - 1] = '\0';
	chomp(buf);
	return strlen(buf);
}
