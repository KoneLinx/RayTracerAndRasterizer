// Ray.h - Basic representation of geometric ray.

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
#include "JLLine.h"

namespace JL
{

	template <int N, typename T>
	struct Ray final : public Line<N, T>
	{
		using Line = Line<N, T>;
		using typename Line::Value;
		using typename Line::Point;
		using typename Line::Vector;

		static constexpr Value tMax = FLT_MAX;
		static constexpr Value tMin = 1e-3f;

		Ray() = default;

		Ray(const Point & origin)
			: Line{ origin, Vector{} }
		{}

		Ray(const Point& origin, const Point& to)
			: Line{ origin, to }
		{}

		Ray(const Point& origin, const Vector& direction)
			: Line{ origin, direction }
		{}

		Ray(const Line& line)
			: Line{ line }
		{}

		Ray(Line&& line) // move
			: Line{ std::move(line) }
		{}

	};

	

}