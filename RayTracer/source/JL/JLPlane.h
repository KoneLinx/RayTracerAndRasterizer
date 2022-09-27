// Plane.h - Basic representation of geometric plane.

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
#include "JLBaseIncludes.h"

namespace JL
{
	template<int N, typename T>
	struct Plane
	{
		using Value = T;
		using Point = Point<N, T>;
		using Vector = Vector<N, T>;

		Point origin;
		Vector normal;

		Plane() = default;

		Plane(const Point& origin, const Vector& normal)
			: origin{ origin }
			, normal{ /*GetNormalized*/(normal) }
		{}

		// Chech handedness of coordinate system !!!
		Plane(const Point& a, const Point& b, const Point& c)
			: origin{ a }
			, normal{ /*GetNormalized*/(Cross(b - a, c - a)) }
		{}

	};
}