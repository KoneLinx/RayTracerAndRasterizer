// ReadFromIstream.h - provides additional istream functionallity

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

#include <iostream>
#include <stdexcept>
#include <string>

namespace JL
{
	
	namespace Read_detail
	{
	
		template<typename ReadType>
		auto StringTo(const std::string& str)
		{
			using namespace std;
		//	if constexpr (is_same<ReadType, string_view			>::value) { return string_view{ str }; } else
			if constexpr (is_same<ReadType, string				>::value) { return str	      ; } else
			if constexpr (is_same<ReadType, char				>::value) { return str[0]     ; } else
			if constexpr (is_same<ReadType, int					>::value) { return stoi  (str); } else
			if constexpr (is_same<ReadType, long				>::value) { return stol  (str); } else
			if constexpr (is_same<ReadType, unsigned long		>::value) { return stoul (str); } else
			if constexpr (is_same<ReadType, long long			>::value) { return stoll (str); } else
			if constexpr (is_same<ReadType, unsigned long long	>::value) { return stoull(str); } else
			if constexpr (is_same<ReadType, float				>::value) { return stof  (str); } else
			if constexpr (is_same<ReadType, double				>::value) { return stod  (str); } else
			if constexpr (is_same<ReadType, long double			>::value) { return stold (str); }
		}
		
		template <typename ReadType>
		using StringToResult_t =
			typename std::enable_if<
				!std::is_same<
					typename std::invoke_result<
						decltype(
							StringTo<ReadType>
						),
						std::string
					>::type,
					void
				>::value,
				ReadType
			>::type;
	
		//template<typename ReadType, typename Char>
		//auto Probe(std::basic_istream<Char>& is, ReadType& var)
		//	-> std::enable_if_t<std::is_void_v<StringToResult_t<ReadType>>>
		//{
		//	std::string input{};
		//	is >> input;
		//	var = StringTo<ReadType>(input);
		//}

		template<typename T, typename ...>
		using requires_t = T;

		template<typename ReadType, typename Char>
		auto Probe(std::basic_istream<Char>& is, ReadType& var)
			-> requires_t<ReadType&, decltype(is >> var)>
		{
			is >> var;
			return var;
		}
	
	};
	
	template<typename ReadType, typename Char>
	typename auto Read(std::basic_istream<Char>& is, ReadType& val)
		-> decltype(Read_detail::Probe(is, val))
	{
		return Read_detail::Probe(is, val);
	}
	
	template<typename ReadType, typename Char>
	typename auto Read(std::basic_istream<Char>& is)
		-> std::decay_t<decltype(Read(is, std::declval<ReadType&>()))>
	{
		ReadType val{};
		return Read(is, val);
	}

}