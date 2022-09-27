// Line.h - Basic representation of geometric line.

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
	template <int N, typename T>
	struct Line
	{
		using Value = T;
		using Point = Point<N, T>;
		using Vector = Vector<N, T>;

		Point origin;
		Vector direction;

		Line()
			: origin{}
			, direction{}
		{}

		Line(const Point& origin, const Vector& direction)
			: origin{ origin }
			, direction{ /*GetNormalized(*/direction }
		{}

		Line(const Point& a, const Point& b)
			: origin{ a }
			, direction{ /*GetNormalized(*/b - a }
		{}

		Line(const Line& line)
			: origin{ line.origin }
			, direction{ line.direction }
		{}
		Line(Line&& line) // move
			: origin{ std::forward(line.origin) }
			, direction{ std::forward(line.direction) }
		{}

		Point operator () (const Value& t) const
		{
			return origin + direction * t;
		}

	};

}