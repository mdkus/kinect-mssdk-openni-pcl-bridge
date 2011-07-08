//@COPYRIGHT@//
//
// Copyright (c) 2011, Tomoto S. Washio
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

#ifndef _VEC_H_
#define _VEC_H_

#include "common.h"
#include "math.h"

/**
 * XnVector3D + vector manipulation methods.
 *
 * Not everything is covered. I'll add features on-demand basis.
 * 
 * @ja
 * XnVector3Dにベクタ演算機能を追加したもの。
 * オンデマンドに機能追加していく。
 *
 * 基本的に結果は戻り値で新しいオブジェクトとして返す。
 * Mで終わるのはthisをmutateする。
 */
class XV3 : public XnVector3D {
public:
	// ctors
	XV3() { X = Y = Z = 0; }
	XV3(float x, float y, float z) { X=x, Y=y, Z=z; }
	XV3(float* v) { X=v[0], Y=v[1], Z=v[2]; }
	XV3(const XnVector3D& v) { X=v.X, Y=v.Y, Z=v.Z; }

	// object lifecycle

	XV3& assign(float x, float y, float z) { X=x, Y=y, Z=z; return *this; }
	XV3& assign(float* v) { X=v[0], Y=v[1], Z=v[2]; return *this; }
	XV3& assign(const XnVector3D& v) { X=v.X, Y=v.Y, Z=v.Z; return *this; }
	XV3& operator=(const XnVector3D& v) { assign(v); return *this; }

	// add, sub, mul, div

	XV3& operator+=(const XnVector3D& v) { X+=v.X, Y+=v.Y, Z+=v.Z; return *this; }
	XV3 operator+(const XnVector3D& v) const { return XV3(X+v.X, Y+v.Y, Z+v.Z); }

	XV3& operator-=(const XnVector3D& v) { X-=v.X, Y-=v.Y, Z-=v.Z; return *this; }
	XV3 operator-(const XnVector3D& v) const { return XV3(X-v.X, Y-v.Y, Z-v.Z); }

	XV3& operator*=(float a) { X*=a, Y*=a, Z*=a; return *this; }
	XV3 operator*(float a) const { return XV3(X*a, Y*a, Z*a); }

	XV3& operator/=(float a) { X/=a, Y/=a, Z/=a; return *this; }
	XV3 operator/(float a) const { return XV3(X/a, Y/a, Z/a); }

	XV3 operator-() const { return XV3(-X, -Y, -Z); }

	// products

	float dot(const XnVector3D& v) const { return X*v.X + Y*v.Y + Z*v.Z; }

	float dotNormalized(const XV3& v) const { return dot(v) / magnitude() / v.magnitude(); }

	XV3& crossM(const XnVector3D& v) { assign(Y*v.Z-Z*v.Y, Z*v.X-X*v.Z, X*v.Y-Y*v.X); return *this; }
	XV3 cross(const XnVector3D& v) const { return XV3(Y*v.Z-Z*v.Y, Z*v.X-X*v.Z, X*v.Y-Y*v.X); }

	// magnitudes

	/** squared magnitude */
	float magnitude2() const { return X*X + Y*Y + Z*Z; }

	float magnitude() const { return sqrt(magnitude2()); }

	/** squared distance */
	float distance2(const XnVector3D& v) const { return (*this - v).magnitude2(); }

	float distance(const XnVector3D& v) const { return sqrt(distance2(v)); }

	// normalizations

	XV3& normalizeM()
	{
		float m = magnitude();
		return assign(X/m, Y/m, Z/m);
	}

	XV3 normalize() const
	{
		float m = magnitude();
		return XV3(X/m, Y/m, Z/m);
	}

	// other derivables

	XV3& interpolateM(const XnVector3D& v, float alpha = 0.5f)
	{
		return assign(::interpolate(X, v.X, alpha), ::interpolate(Y, v.Y, alpha), ::interpolate(Z, v.Z, alpha));
	}

	XV3 interpolate(const XnVector3D& v, float alpha = 0.5f) const
	{
		return XV3(::interpolate(X, v.X, alpha), ::interpolate(Y, v.Y, alpha), ::interpolate(Z, v.Z, alpha));
	}
};

/**
 * Convenient wrapper to convert XV3 to M3DVector3f for GLTools.
 *
 * e.g.
 * XV3 v;
 * GLFrame frame;
 * frame.SetForwardVector(XV3toM3D(v));
 */
class XV3toM3D
{
private:
	M3DVector3f m3d;

public:
	XV3toM3D(const XV3& v) {
		m3d[0]=v.X, m3d[1]=v.Y, m3d[2]=v.Z;
	}
	
	operator float*() {
		return m3d;
	}

	operator const float*() const {
		return m3d;
	}
};

#endif
