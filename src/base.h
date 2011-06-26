#pragma once
#include <Windows.h>
#include <MSR_NuiApi.h>
#include <XnCppWrapper.h>
#include <XnModuleCppInterface.h>
#include <XnEvent.h>

#pragma warning(disable : 4996)

struct XnStatusException {
	const XnStatus nStatus;
	XnStatusException(XnStatus n) : nStatus(n) {}
};

#define CHECK_XN_STATUS(statement) \
{ \
	XnStatus __status = (statement); \
	if (__status != XN_STATUS_OK) throw XnStatusException(__status); \
}

struct Win32ResultException {
	const HRESULT hResult;
	Win32ResultException(HRESULT hr) : hResult(hr) {}
};

#define CHECK_HRESULT(statement) \
{ \
	HRESULT __hResult = (statement); \
	if (FAILED(__hResult)) throw Win32ResultException(__hResult); \
}
