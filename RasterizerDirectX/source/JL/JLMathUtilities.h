// MathUtilities.h - Math

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
#include <cmath>
#include <utility>

namespace JL
{

	template <typename Value>
	Value Sqrt(const Value& value)
	{
		return static_cast<Value>(std::sqrt(value));
	}

	namespace quadratic
	{

		template <typename Value>
		constexpr Value Discriminant(const Value& a, const Value& b, const Value& c)
		{
			return b * b - (a * c * static_cast<Value>(4));
		}

		template <typename Value>
		constexpr Value TFrontDRooted(const Value& a, const Value& b, const Value& dRoot)
		{
			return (-b - dRoot) / (a * 2);
		}

		template <typename Value>
		constexpr Value TBackDRooted(const Value& a, const Value& b, const Value& dRoot)
		{
			return (-b + dRoot) / (a * 2);
		}

		template <typename Value>
		Value TFrontD(const Value& a, const Value& b, const Value& d)
		{
			return TFrontDRooted(a, b, Sqrt(d));
		}

		template <typename Value>
		Value TBackD(const Value& a, const Value& b, const Value& d)
		{
			return TBackDRooted(a, b, Sqrt(d));
		}

		template <typename Value>
		constexpr std::pair<Value, Value> TDRooted(const Value& a, const Value& b, const Value& dRoot)
		{
			return { TFrontDRooted(a, b, dRoot), tBackDRooted(a, b, dRoot) };
		}

		template <typename Value>
		std::pair<Value, Value> TD(const Value& a, const Value& b, const Value& d)
		{
			const Value dRoot{ Sqrt(d) };
			return { TFrontDRooted(a, b, dRoot), TBackDRooted(a, b, dRoot) };
		}

		template <typename Value>
		std::pair<Value, Value> T(const Value& a, const Value& b, const Value& c)
		{
			const Value d{ Discriminant(a,b,c) };
			const Value dRoot{ Sqrt(d) };
			return { TFrontDRooted(a, b, dRoot), TBackDRooted(a, b, dRoot) };
		}

	}

}