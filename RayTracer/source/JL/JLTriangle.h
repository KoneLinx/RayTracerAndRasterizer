// Triangle.h - Basic representation of geometric triangle.

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
#include "JLPolygon.h"

namespace JL
{

	template<typename T>
	struct Polygon<3, 3, T>
	{
		using Triangle = Polygon<3, 3, T>;
		using Value = T;
		using Point = Point<3, T>;
		using Vector = Vector<3, T>;

		union
		{
			Point data[3];
			struct
			{
				Point a, b, c;
			};
		};
		Vector normal;

		Polygon<3, 3, T>() = default;

		Polygon<3, 3, T>(const Point& a, const Point& b, const Point& c, const Vector& normal)
			: a{ a } , b{ b } , c{ c }
			, normal{ normal }
		{}

		Polygon<3, 3, T>(const Point& a, const Point& b, const Point& c)
			: a{ a } , b{ b } , c{ c }
			, normal{ GetNormalized(Cross(b - a, c - a)) }
		{}

	};

	//template<typename T>
	//using Triangle = Polygon<3, 3, T>;

}