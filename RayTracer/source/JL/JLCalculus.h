// Calculus.h - Calculus implementations for geometric objects.

/* Copyright (C) 2020 Kobe Vrijsen

   this file is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3.0 of the License, or (at your option) any later version.

   This file is made available in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see
   <https://www.gnu.org/licenses/>.

   Information in regards to this file:
   Contact:   kobevrijsen@posteo.be
*/

#pragma once
#include "JLGeometry.h"
#include "JLMesh.h"
#include <algorithm>

namespace JL
{

	// enums dont work as flags
	struct CullFlag
	{
		static constexpr unsigned single  = false << 1u;
		static constexpr unsigned both    = true  << 1u;
		static constexpr unsigned enabled = true ;
		static constexpr unsigned front   = true ;
		static constexpr unsigned back    = false;
		static constexpr unsigned any     = both | enabled;
	};


	struct CullMode
	{
		using Flag = unsigned;
		static constexpr unsigned none  = CullFlag::both   | !CullFlag::enabled;
		static constexpr unsigned both  = CullFlag::both   |  CullFlag::enabled;
		static constexpr unsigned front = CullFlag::single |  CullFlag::front  ;
		static constexpr unsigned back  = CullFlag::single | !CullFlag::front  ;
	};

	template<CullMode::Flag cullmode = CullMode::front, int N, typename T, typename = void >
	bool Intersect(Intersection<N, T>& result, const Line<N, T>& line, const Plane<N, T>& plane)
	{
		if constexpr (cullmode == CullMode::none)
		{
			return false;
		}
		else
		{

			const T divisor{ Dot(line.direction, plane.normal) };

			// implement culling
			//if (divisor > 0)
			//	return false;

			if (Conditional<cullmode & CullFlag::both>(
				divisor == static_cast<T>(0),
				Conditional<cullmode & CullFlag::front>(
					divisor > static_cast<T>(0),
					divisor < static_cast<T>(0)
				)
			))
			{
				return false;
			}
			else
			{

				const T t{ Dot(plane.origin - line.origin, plane.normal) / divisor };
				result = t;
				return true;
			
			}

		}
	}

	template<CullMode::Flag  cullmode = CullMode::front, int N, typename T, typename = void>
	bool Intersect(Intersection<N, T>& result, const Line<N, T>& line, const Circular<N, T>& round)
	{
		if constexpr (cullmode == CullMode::none)
		{
			return false;
		}
		else
		{

			const typename Line<N, T>::Vector distance{ line.origin - round.center };
			const T a{ Dot(line.direction, line.direction) };
			const T b{ 2 * Dot(line.direction, distance) };
			const T c{ Dot(distance, distance) - (round.radius * round.radius) };
			const T d{ quadratic::Discriminant(a, b, c) };

			if (d < static_cast<T>(0))
				return false;

			if constexpr (cullmode & CullFlag::both)
			{
				const T t1 = quadratic::TFrontD(a, b, d);
				const T t2 = quadratic::TBackD(a, b, d);
				// incorrect for rays
				const T t = std::min(t1, t2);
				result = t;
			}
			else
			{
				const T t{ Conditional<cullmode & CullFlag::front>(quadratic::TFrontD<T>, quadratic::TBackD<T>)(a, b, d) };
				result = t;
			}

			return true;

		}
	}

	template<CullMode::Flag cullmode = CullMode::front, int N, typename T, typename = void>
	bool Intersect(Intersection<N, T>& result, const Line<N, T>& line, typename Mesh<N, T>::MeshData::Triangle const& triangle)
	{
		if constexpr (cullmode == CullMode::none)
		{
			return false;
		}
		else
		{

			Intersection<N, T> intersection{};
			Plane<N, T> plane{ *triangle.x, *triangle.y, *triangle.z };
			if (!Intersect<cullmode, N, T, void>(intersection, line, plane))
				return false;

			Point<N, T> const pos{ line(intersection) };
			Vector<N, T> const distance{ pos - *triangle.x };

			Vector<N, T> const side1{ *triangle.y - *triangle.x };
			if (Dot(plane.normal, Cross(side1, distance)) < 0)
				return false;

			Vector<N, T> const side2{ *triangle.z - *triangle.x };
			if (Dot(plane.normal, Cross(side2, distance)) > 0)
				return false;

			Vector<N, T> const distance3{ pos - *triangle.y };
			Vector<N, T> const side3{ *triangle.z - *triangle.y };
			if (Dot(plane.normal, Cross(side3, distance3)) < 0)
				return false;

			result = intersection;
			result.hitFace = &triangle;
			return true;

		}
	}

	template<CullMode::Flag cullmode = CullMode::front, int N, typename T, typename = void>
	bool Intersect(Intersection<N, T>& result, const Line<N, T>& line, typename Mesh<N, T> const& mesh)
	{
		return std::any_of(
			mesh.begin(), mesh.end(),
			[&result, &line](auto const& triangle)
			{
				return Intersect<cullmode, N, T, void>(result, line, triangle);
			}
		);
	}

	template<CullMode::Flag cullmode = CullMode::front, bool behind = false, int N, typename T, template<int, typename> typename Object>
	bool Intersect(Intersection<N, T>& result, const Ray<N, T>& ray, const Object<N, T>& object)
	{
		if constexpr(!behind)
			return Intersect<cullmode, N, T, void>(result, ray, object) && result >= ray.tMin && result < ray.tMax;
		else
			return Intersect<cullmode, N, T, void>(result, ray, object) && result <= -ray.tMin && result > -ray.tMax;
	}

	template<bool behind = false, int N, typename T, template<int, typename> typename Object>
	bool Intersect(Intersection<N, T>& result, const Ray<N, T>& ray, const Object<N, T>& object, CullMode::Flag cullmode)
	{
		switch (cullmode)
		{
		case CullMode::front:
			return Intersect<CullMode::front, behind, N, T, Object>(result, ray, object);
		case CullMode::none:
			return false;
		case CullMode::both:
			return Intersect<CullMode::both, behind, N, T, Object>(result, ray, object);
		case CullMode::back:
			return Intersect<CullMode::back, behind, N, T, Object>(result, ray, object);
		}
	}

	template<bool behind = false, int N, typename T, template<int, typename> typename Object>
	bool Intersect(const Ray<N, T>& ray, const Object<N, T>& object, CullMode::Flag cullmode)
	{
		Intersection<N, T> t;
		return Intersect<behind, N, T, Object>(t, ray, object, cullmode);
	}

	template<int N, typename T>
	Vector<N, T> HalfVector(Vector<N, T> const& a, Vector<N, T> const& b)
	{
		return (a + b) * 0.5f;
	}

}