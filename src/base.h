#pragma once
#include <Windows.h>
#include <MSR_NuiApi.h>
#include <XnCppWrapper.h>
#include <XnModuleCppInterface.h>
#include <XnEvent.h>

#pragma warning(disable : 4996)

struct XnStatusException {
	const XnStatus nStatus;
	const HRESULT hResult;

	XnStatusException(XnStatus n, HRESULT hr = S_OK) : nStatus(n), hResult(hr) {}
};

#define CHECK_XN_STATUS(statement) \
{ \
	XnStatus __status = (statement); \
	if (__status != XN_STATUS_OK) { fprintf(stderr, "%s", #statement); throw XnStatusException(__status); } \
}

#define CHECK_HRESULT(statement) \
{ \
	HRESULT __hr = (statement); \
	if (FAILED(__hr)) { fprintf(stderr, "%s", #statement); throw XnStatusException(XN_STATUS_ERROR, __hr); } \
}
