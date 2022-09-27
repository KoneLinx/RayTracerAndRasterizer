// MathExtra.h - Extra utility functions for linear algebra
/*
Author: Kobe Vrijsen

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#pragma once
#include "../EMath.h"

namespace JL
{
	// utilizing the Elite library for basic linear algebra implementations
	using namespace Elite;

	// EMath suggestions :)


	// Please note:  Vector::Normalize  is very slow.

	// Rotate a vector or point by an equally sized matrix (basic multiplication)
	
	template<int N, typename T, template<int, typename ...> typename Object>
	inline Object<N, T> operator *(const Matrix<N, N, T>& m, const Object<N, T>& v)
	{
		Object<N, T> out{};
		for (int i{}; i < N; ++i)
			for (int j{}; j < N; ++j)
				out[i] += m(i, j) * v[j];
		return out;
	}

	/*
	template<int N = 3, typename T, template<int, typename ...> typename Object>
	inline Object<3, T> operator *(const Matrix<3, 3, T>& m, const Object<3, T>& v)
	{
		return Object<3, T>{
			m(0, 0)* v.x + m(0, 1) * v.y + m(0, 2) * v.z,
			m(1, 0)* v.x + m(1, 1) * v.y + m(1, 2) * v.z,
			m(2, 0)* v.x + m(2, 1) * v.y + m(2, 2) * v.z
		};
	}
	*/


	// in place transformation of vectors and points with a transformarion matrix

	template<typename T, template<int, typename ...> typename Object>
	inline Object<3, T>& operator *= (Object<3, T>& v, const Matrix<3, 3, T>& m)
	{
		const T x{ m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z };
		const T y{ m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z };
		v.z =      m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z  ;
		v.y = y;
		v.x = x;
		return v;
	}

	template<typename T>
	inline Vector<3, T>& operator *= (Vector<3, T>& v, const Matrix<4, 4, T>& m)
	{
		const T x{ m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z };
		const T y{ m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z };
		v.z =      m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z  ;
		v.y = y;
		v.x = x;
		return v;
	}

	template<typename T>
	inline Vector<3, T>& operator *= (Point<3, T>& v, const Matrix<4, 4, T>& m)
	{
		const T x{ m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z + m(0, 3) };
		const T y{ m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z + m(1, 3) };
		v.z =      m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z + m(2, 3)  ;
		v.y = y;
		v.x = x;
		return v;
	}

	template<typename T, template<int, typename ...> typename Object>
	inline Object<4, T>& operator *= (Object<4, T>& v, const Matrix<4, 4, T>& m)
	{
		const T x{ m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z + m(0, 3) * v.w };
		const T y{ m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z + m(1, 3) * v.w };
		v.z =      m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z + m(2, 3) * v.w  ;
		v.y = y;
		v.x = x;
		return v;
	}

	template<int N, typename T, template<int, typename ...> typename Object>
	inline Object<N, T>& operator *= (Object<N, T>& v, const Matrix<N, N, T>& m)
	{
		return v = std::move(m * v);
	}


	// construct Unit matrix for given X axis

	template<typename T>
	inline Matrix<3, 3, T> MakeRotation(const Vector<3, T>& newX, const Vector<3, T>& up)
	{
		const Vector<3, T> x{ GetNormalized(newX) };
		const Vector<3, T> z{ GetNormalized(Cross(x, up)) };
		const Vector<3, T> y{ GetNormalized(Cross(z, x )) };
		return Matrix<3, 3, T>{ x, y, z };
	}

	template<typename T>
	inline Matrix<3, 3, T> MakeRotation(const Vector<3, T>& newX)
	{
		Vector<3, T> up{};
		up[1] = static_cast<T>(1);
		return MakeRotation(newX, up);
	}


	// rotation around given axis and angle

	template<typename T>
	inline Matrix<3, 3, T> MakeRotation(const Vector<3, T>& axis, const T& angle)
	{
		const Vector<3, T> up{ 0, 1, 0 };
		if (axis == up)
			return Elite::MakeRotationY(angle);
		const Matrix<3, 3, T> unit{ MakeRotation(axis, up) };
		return unit * MakeRotationX(angle) * Inverse(unit);
	}


	// Scale x, y, and z component of a matrix with a vectors's x, y, and z
	
	template<typename T>
	Matrix<3, 3, T>& Scale(Matrix<3, 3, T>& matrix, const Vector<3, T>& scale)
	{
		matrix[0] *= scale.x;
		matrix[1] *= scale.y;
		matrix[2] *= scale.z;
		return matrix;
	}

	template<int N, typename T>
	Matrix<N, N, T>& Scale(Matrix<N, N, T>& matrix, const Vector<N, T>& scale)
	{
		for (size_t i{}; i < N; ++i)
			matrix[i] *= scale[i];
		return matrix;
	}

	template<int N, typename T>
	Matrix<N, N, T> GetScaled(const Matrix<N, N, T>& matrix, const Vector<N, T>& scale)
	{
		Matrix<N, N, T> out{ matrix };
		Scale(out, scale);
		return out;
	}

	template<typename T, template<int, typename> typename Object>
	Vector<3, T>& Scale(Object<3, T>& vector, const Object<3, T>& scale)
	{
		vector.x *= scale.x;
		vector.y *= scale.y;
		vector.z *= scale.z;
		return vector;
	}


	template<int N, typename T, template<int, typename> typename Object>
	Vector<N, T>& Scale(Object<N, T>& vector, const Object<N, T>& scale)
	{
		for (size_t i{}; i < N; ++i)
			vector[i] * scale[i];
		return vector;
	}

	template<int N, typename T>
	Vector<N, T> inline GetScaled(const Vector<N, T>& vector, const Vector<N, T>& scale)
	{
		Vector<N, T> out{ vector };
		Scale(out, scale);
		return out;
	}

	
	// Project vector to plane

	template<int N, typename T>
	Vector<N, T> Project(Vector<N, T> const& vector, Vector<N, T> const& a, Vector<N, T> const& b)
	{
		return Elite::Project(vector, a) + Elite::Project(vector, b);
	}

	template<int N, typename T>
	Vector<N, T> ProjectNormal(Vector<N, T> const& vector, Vector<N, T> const& normal)
	{
		return vector - Elite::Project<N, T>(vector, normal);
	}


	// Scale vector

	template<int N, typename T>
	Vector<N, T>& operator *= (Vector<N, T> & vector, T const& scale)
	{
		for (size_t i{}; i < N; ++i)
			vector.data[i] *= scale;
		return vector;
	}

	template<typename T>
	Vector<3, T>& operator *= (Vector<3, T>& vector, T const& scale)
	{
		vector.x *= scale;
		vector.y *= scale;
		vector.z *= scale;
		return vector;
	}

	template<int N, typename T, typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Vector<N, T>& operator /= (Vector<N, T>& vector, U const& scale)
	{
		for (size_t i{}; i < N; ++i)
			vector.data[i] /= scale;
		return vector;
	}

	template<typename T, typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Vector<3, T>& operator /= (Vector<3, T>& vector, U const& scale)
	{
		vector.x /= scale;
		vector.y /= scale;
		vector.z /= scale;
		return vector;
	}


	// float - Vector

	template<int N, typename T, typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Vector<N, T>& operator - (U const& value, Vector<N, T> const& vector)
	{
		Vector<N, T> out{};
		for (size_t i{}; i < N; ++i)
			out.data[i] = vector.data[i] - value;
		return out;
	}

	template<typename T, typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Vector<3, T> operator - (U const& value, Vector<3, T> const& vector)
	{
		return Vector<3, T>{
			vector.x - value,
			vector.y - value,
			vector.z - value
		};
	}

}

using JL::operator*=;