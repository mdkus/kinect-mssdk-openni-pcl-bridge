#pragma once
#include "base.h"
#include "Properties.h"

#define DEFINE_BINARY_PROPERTY(name, size, ...) \
	const BYTE s_##name##Data[] = { __VA_ARGS__ }; \
	SetGeneralProperty(#name, size, s_##name##Data);

class PrimeSenseProperties : public Properties
{
private:
	void* m_instancePointer;

public:
	PrimeSenseProperties()
	{
		// empirical values
		SetIntProperty("MaxShift", 2047);
		SetIntProperty("ZPD", 120);
		SetIntProperty("ConstShift", 200);
		SetRealProperty("ZPPS", 0.1052); // 0.1042?
		SetRealProperty("LDDIS", 7.5);
		SetGeneralProperty("InstancePointer", sizeof(m_instancePointer), (const BYTE*)(&m_instancePointer));

#include "PrimeSenseBinaryPropertyDef.h"
	}

	void SetInstancePointer(void* instancePointer)
	{
		m_instancePointer = instancePointer;
	}
};
