#pragma once
#include "base.h"
#include "Properties.h"

#define DEFINE_BINARY_PROPERTY(name, size, ...) \
	const BYTE s_##name##Data[] = { __VA_ARGS__ }; \
	SetGeneralProperty(#name, size, s_##name##Data);

class PrimeSenseProperties : public Properties
{
public:
	PrimeSenseProperties()
	{
		// empirical values
		SetIntProperty("MaxShift", 2047);
		SetIntProperty("ZPD", 120);
		SetIntProperty("ConstShift", 200);
		SetRealProperty("ZPPS", 0.1052);
		SetRealProperty("LDDIS", 7.5);

#include "PrimeSenseBinaryPropertyDef.h"
	}
};
