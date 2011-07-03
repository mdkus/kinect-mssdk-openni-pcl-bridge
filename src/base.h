#pragma once
#include <Windows.h>
#include <MSR_NuiApi.h>
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