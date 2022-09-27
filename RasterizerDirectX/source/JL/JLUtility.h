// Utility.h - Utilities

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
#include <cstring>
#include "JLVisitor.h"

namespace JL
{

	template<bool TEST, typename TrueType, typename FalseType>
	constexpr std::conditional_t<TEST, TrueType const&, FalseType const&> Conditional(TrueType const& trueValue, FalseType const& falseValue) noexcept
	{
		if constexpr (TEST)
			return trueValue;
		else
			return falseValue;
	}

	template<bool TEST, typename TrueType, typename FalseType>
	constexpr std::conditional_t<TEST, TrueType, FalseType const&> Conditional(TrueType&& trueValue, FalseType const& falseValue) noexcept
	{
		if constexpr (TEST)
			return trueValue;
		else
			return falseValue;
	}

	template<bool TEST, typename TrueType, typename FalseType>
	constexpr std::conditional_t<TEST, TrueType const&, FalseType> Conditional(TrueType const& trueValue, FalseType&& falseValue) noexcept
	{
		if constexpr (TEST)
			return trueValue;
		else
			return falseValue;
	}

	template<bool TEST, typename TrueType, typename FalseType>
	constexpr std::conditional_t<TEST, TrueType const&, FalseType const&> Conditional(TrueType&& trueValue, FalseType&& falseValue) noexcept
	{
		if constexpr (TEST)
			return trueValue;
		else
			return falseValue;
	}

	template<typename Type>
	constexpr Type Max(Type const& first, Type const& second)
	{
		return std::max(first, second);
	}

	template<typename Type, typename ... Rest>
	constexpr Type Max(Type const& first, Type const& second, Rest const& ... rest)
	{
		return Max<Type>(std::max(first, second), rest...);
	}

	template<typename Type>
	constexpr Type Min(Type const& first, Type const& second)
	{
		return std::min(first, second);
	}

	template<typename Type, typename ... Rest>
	constexpr Type Min(Type const& first, Type const& second, Rest const& ... rest)
	{
		return Min<Type>(std::min(first, second), rest...);
	}

	template<typename Type, typename ... Values>
	constexpr auto Minmax(Type const& first, Values const& ... values)
	{
		return std::pair{
			Min<Type>(first, values ...),
			Max<Type>(first, values ...)
		};
	}

	template<typename T>
	constexpr T const& Nullref()
	{
		return *static_cast<std::decay_t<T> const* const>(nullptr);
	}

	struct IdentityFunctor
	{
		template <typename T>
		auto const& operator () (T const& t) const { return t;}
		template <typename T>
		auto operator () (T && t) const { return t; }
	};

	namespace detail
	{

		template<typename T, bool>
		struct Wrapper;

		template<typename T>
		struct Wrapper<T, false>
		{
			T value;

			constexpr Wrapper() noexcept = default;
			constexpr Wrapper(T value)
				: value{ value }
			{}

			constexpr operator T&() noexcept
			{
				return value;
			}
			constexpr operator T() const noexcept
			{
				return value;
			}
		};

		template<typename T>
		struct Wrapper<T, true>
		{
			T value;

			constexpr Wrapper() noexcept = default;
			constexpr Wrapper(T value)
				: value{ value }
			{}

			constexpr operator T() noexcept
			{
				return value;
			}

			constexpr std::remove_pointer_t<T> &
				operator * () noexcept
			{
				return *value;
			}
			constexpr std::remove_pointer_t<T> const&
				operator * () const noexcept
			{
				return *value;
			}

			constexpr T
				operator -> () noexcept
			{
				return value;
			}
			constexpr std::remove_pointer_t<T> const*
				operator -> () const noexcept
			{
				return value;
			}
		};

	}

	template<typename T>
	using Wrapper = detail::Wrapper<T, std::is_pointer_v<T>>;

	template<typename R, typename T >
	auto MemberGetter(R T::* member) noexcept
	{
		return Visitor{
			[member = member](T const& object) -> R const&
			{
				return object.*(member);
			},
			[member = member](T& object) -> R&
			{
				return object.*(member);
			}
		};
	}

	template<typename R, typename T >
	auto MemberTester(R T::* member, R const& value) noexcept
	{
		return (
			[member = member, value = value](T const& object) -> bool
			{
				return value == object.*(member);
			}
		);
	}

	template<bool TEST, typename True, typename False>
	constexpr auto const& Conditional(True const& t, False const& f)
	{
		if constexpr (TEST)
			return t;
		else
			return f;
	}

	template<typename A, typename B>
	int memcmp(A const& a, B const& b) noexcept
	{
		constexpr auto size = std::min(sizeof(A), sizeof(B));
		return std::memcmp(&a, &b, size);
	}

	template<typename Value>
	struct LessMemcmp
	{
		bool operator () (Value const& a, Value const& b) const noexcept
		{
			return JL::memcmp(a, b) < 0;
		}
	};

	template<typename Value, template<typename> typename Cmp>
	struct PtrCmp
	{
		constexpr static Cmp<Value> const& cmp{};
		bool operator () (Value const* a, Value const* b) const noexcept
		{
			return cmp(*a, *b);
		}
	};

	template <typename T, size_t SIZE>
	void swap(T(&a)[SIZE], T(&b)[SIZE])
	{
		std::swap_ranges(
			std::begin(a), std::end(a),
			std::begin(b)
		);
	}

}