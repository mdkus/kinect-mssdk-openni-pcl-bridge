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
#include <tchar.h>

class GlobalConfig
{
private:
	static GlobalConfig* cs_pInstance;

public:
	static GlobalConfig* getInstance() // throws XnStatusException
	{
		if (!cs_pInstance) {
			cs_pInstance = new GlobalConfig();
		}
		return cs_pInstance;
	}

private:
	BOOL m_useConnectionIdForCreationInfo;

public:
	GlobalConfig() : m_useConnectionIdForCreationInfo(0)
	{
		// FIXME quick hack!

		extern HMODULE g_hModule;
		TCHAR path[MAX_PATH+1];
		DWORD len = GetModuleFileName(g_hModule, path, MAX_PATH+1);

		if (len < 4) {
			puts("Cannot get the module file name of the bridge DLL. Ignoring the global config.");
			return;
		}

		for (TCHAR* p = path; *p; p++) {
			*p = tolower(*p);
		}
		
		if (replaceSuffix(path, _T("debug.dll"), _T(".ini")) ||
				replaceSuffix(path, _T("release.dll"), _T(".ini")) ||
				replaceSuffix(path, _T(".dll"), _T(".ini"))) {
			// we are good
		} else {
			puts("The module file name is not as supposed to be. Ignoring the global config.");
			return;
		}

		// wprintf(_T("%s\n"), path);

		readConfig(path);
	}

	BOOL isUseConnectionIdForCreationInfo() { return m_useConnectionIdForCreationInfo; }

private:
	static BOOL replaceSuffix(TCHAR* s, const TCHAR* a, const TCHAR* b)
	{
		// len(a) should be >= len(b)
		TCHAR* p = s + _tcslen(s) - _tcslen(a);
		if (_tcscmp(p, a) == 0) {
			_tcscpy(p, b);
			return TRUE;
		} else {
			return FALSE;
		}
	}

	void readConfig(const TCHAR* path)
	{
		m_useConnectionIdForCreationInfo = GetPrivateProfileInt(
			_T("general"), _T("useConnectionIdForCreationInfo"), m_useConnectionIdForCreationInfo, path);
	}
};
