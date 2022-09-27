// Hash.h - Hasing utility.

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

#include <string_view>

namespace JL
{

	// Simple string hashes. Only use for unsignificant data, Never for security.

	using SimpleHash_t = size_t;

	constexpr SimpleHash_t SimpleHash(std::basic_string_view<char> string) noexcept
	{
		size_t hash{};
		for (size_t i{}; i < string.size(); ++i)
			hash ^= size_t(string[i]) << (i % sizeof(size_t)) * 8 * sizeof(char);
		return hash;
	}

	constexpr SimpleHash_t operator "" _SimpleHash (const char* str, size_t size) noexcept
	{
		return SimpleHash({ str, size - 1 });
	}

	struct SimpleHasher
	{
		using Hash_t = SimpleHash_t;
		constexpr static auto Hash = SimpleHash;

		constexpr auto operator () (std::basic_string_view<char> string) noexcept
		{
			return Hash(string);
		}
	};

}

