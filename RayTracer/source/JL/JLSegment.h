// Segment.h - Basic representation of geometric segment.

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
	template<int N, typename T>
	struct Segment : public Line<N, T>
	{
		using Line = Line<N, T>;
		using typename Line::Value;
		using typename Line::Point;
		using typename Line::Vector;

		Value t0;
		Value t1;

		Segment() = default;

		Segment(const Point& a, const Point& b)
			: Line{ a, b }
			, t0{ static_cast<Value>(0) }
			, t1{ Distance(b,a) }
		{}

		Segment(const Line& line, const Value& length) // t0 = 0
			: Segment(line, static_cast<Value>(0), length)
		{}
		Segment(Line&& line, const Value & length) // move
			: Segment(std::move(line), static_cast<Value>(0), length)
		{}

		Segment(const Line& line, const Value& t0, const Value& t1)
			: Line{ line }
			, t0{ t0 }
			, t1{ t1 }
		{}
		Segment(Line&& line, const Value& t0, const Value& t1) // move
			: Line{ std::move(line) }
			, t0{ t0 }
			, t1{ t1 }
		{}

	};
}