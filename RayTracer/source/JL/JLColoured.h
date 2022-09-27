// Coloured.h - Defines functionality for coloured objects

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
#include <utility>
#include "JL/JLBaseIncludes.h"

namespace JL
{

	template<typename Object, int N = 3, typename Value = float, template<int, typename...> typename Colour = Vector>
	struct Coloured : public Object, public Coloured<void>
	{
		using ColouredBase = Coloured<void, N, Value, Colour>;
		using Colour_t = Colour<N, Value>;

		Coloured() = default;
		
		Coloured(Object&& base, Colour_t const& colour = {}, Value const& intensity = {})
			: Object{ std::move(base) }
			, ColouredBase{ colour, intensity }
		{}

		Coloured(Object const& base, Colour_t const& colour = {}, Value const& intensity = {})
			: Object{ base }
			, ColouredBase{ colour, intensity }
		{}

		//operator ColouredBase() const
		//{
		//	return *this;
		//}

	};

	template<int N, typename Value, template<int, typename...> typename Colour>
	struct Coloured<void, N, Value, Colour>
	{
		static constexpr size_t COLOURS = N;
		using Colour_t = Colour<N, Value>;

		Colour_t colour;
		Value reflectance;

		Coloured<void, N, Value, Colour>() = default;

		Coloured<void, N, Value, Colour>(Colour_t const& colour, Value const& intensity)
			: colour{ colour }
			, reflectance{ intensity }
		{}

		template<typename Object, int N, typename Value, template<int, typename...> typename Colour>
		Coloured& operator = (Coloured<Object, N, Value, Colour> const& other)
		{
			return *this = static_cast<Coloured const&>(other);
		}

	};

	template<int N, typename Value, template<int, typename...> typename Colour>
	struct ColourData
	{
		static constexpr size_t COLOURS = N;
		using Colour_t = Colour<N, Value>;

		Colour_t colour;
		Value reflectance;
	};

}