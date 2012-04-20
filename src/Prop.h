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

#include <propsys.h>

class PropVar : public PROPVARIANT
{
public:
	HRESULT lastResult;

public:
	PropVar() : lastResult(S_OK)
	{
		PropVariantInit(this);
	}

	~PropVar()
	{
		PropVariantClear(this);
	}

	PropVar* Pull(IPropertyStore* ps, const PROPERTYKEY& key)
	{
		lastResult = ps->GetValue(key, this);
		return this;
	}

	bool Push(IPropertyStore* ps, const PROPERTYKEY& key)
	{
		if (vt == VT_EMPTY) {
			lastResult = S_OK;
			return true; // nothing to do
		} else {
			lastResult = ps->SetValue(key, *this);
			return SUCCEEDED(lastResult);
		}
	}

	bool PushIfChanged(IPropertyStore* ps, const PROPERTYKEY& key)
	{
		if (vt != VT_EMPTY) {
			PropVar currentVar;
			currentVar.Pull(ps, key);
			if (!Equals(currentVar)) {
				return Push(ps, key);
			}
		}
		lastResult = S_OK;
		return true;
	}

	bool Equals(const PropVar& other)
	{
		return memcmp((PROPVARIANT*)this, (PROPVARIANT*)&other, sizeof(PROPVARIANT)) == 0;
	}

	void SetInt32(LONG value)
	{
		vt = VT_I4;
		lVal = value;
	}

	void SetBool(BOOL value)
	{
		vt = VT_BOOL;
		boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
	}
};

class Prop
{
public:
	const PROPERTYKEY key;
	PropVar var;

public:
	Prop(const PROPERTYKEY& _key) : key(_key) {}
	~Prop() {}

	bool Push(IPropertyStore* ps) { return var.Push(ps, key); }
	bool PushIfChanged(IPropertyStore* ps) { return var.PushIfChanged(ps, key); }
	PropVar* Pull(IPropertyStore* ps) { return var.Pull(ps, key); }
};
