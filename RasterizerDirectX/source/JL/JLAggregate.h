// Aggregate.h - provides agregate structure

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

namespace JL
{

	template<typename One, typename ... Many>
	using is_any_of = std::bool_constant<(std::is_same_v<One, Many> || ...)>;

	template<typename One, typename ... Many>
	constexpr bool is_any_of_v = is_any_of<One, Many...>::value;

	template<size_t index, typename ... Types>
	using get_t = std::tuple_element_t<index, std::tuple<Types...>>;

	template<typename ... Bases>
	struct Aggregate : public Bases...
	{

		constexpr Aggregate() noexcept = default;

		constexpr Aggregate(Bases const& ... bases)
			: Bases{ bases } ...
		{}

		constexpr Aggregate(Bases && ... bases)
			: Bases{ std::move(bases) } ...
		{}

		template<typename Base, typename = std::enable_if_t<is_any_of_v<Base, Bases...>>>
		constexpr Base & As() noexcept
		{
			return static_cast<Base&>(*this);
		}

		template<typename Base, typename = std::enable_if_t<is_any_of_v<Base, Bases...>>>
		constexpr Base const& As() const noexcept
		{
			return static_cast<Base const&>(*this);
		}

		template<size_t index>
		constexpr auto & Get() noexcept
		{
			return As<get_t<index, Bases...>>();
		}

		template<size_t index>
		constexpr auto const& Get() const noexcept
		{
			return As<get_t<index, Bases...>>();
		}
		
		template<typename T>
		static constexpr bool Has() noexcept
		{
			return is_any_of_v<T, Bases...>;
		}

	};

}