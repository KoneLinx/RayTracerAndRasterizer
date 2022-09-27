// VectorTuple.h - Aggregate typed vector/tuple structure.

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
#include <vector>
#include <tuple>
#include <variant>
#include <algorithm>

namespace JL
{

	template<typename ...Contained>
	class VectorTuple : private std::tuple<std::vector<Contained>...>
	{
		using This = VectorTuple<Contained...>;

		template<typename Type>
		struct HasType : std::bool_constant<(std::is_same<Type, Contained>::value || ...)> {};

		template<typename ...Types>
		using TypeCondition = typename std::enable_if<(HasType<Types>::value && ...)>::type;

		template<size_t index>
		using IndexCondition = typename std::enable_if<(sizeof...(Contained) > index)>::type;

	public:

		template<typename Type>
		using Container = std::vector<Type>;
		using Tuple = std::tuple<Container<Contained>...>;
		using Variant = std::variant<Contained...>;
		using PtrVariant = std::variant<const Contained*...>;

		constexpr VectorTuple() noexcept = default;

		template<typename ...Types, typename = typename std::enable_if<(HasType<Types>::value & ...)>::type>
		// typename = TypeCondition<Types...>
		VectorTuple(Types && ... objects)
			: Tuple{}
		{
			(operator+=(std::move(objects)), ...);
		}

		template<typename Type, typename = TypeCondition<Type>>
		This& operator += (Type const& object)
		{
			std::get<Container<Type>>(AsTuple()).push_back(object);
			return *this;
		}

		template<typename Type, typename = TypeCondition<Type>>
		This& operator += (Type && object)
		{
			std::get<Container<Type>>(AsTuple()).push_back(std::move(object));
			return *this;
		}

		template<typename ...Types, typename = typename std::enable_if<(HasType<Types>::value & ...)>::type>
		// typename = TypeCondition<Types...>
		void add(Types && ... objects)
		{
			(operator+=(objects), ...);
		}

		template<typename Type>
		static constexpr bool Has() noexcept
		{
			return HasType<Type>::value;
		}

		template<typename Type, typename = TypeCondition<Type>>
		constexpr Container<Type> const& Get() const noexcept
		{
			return std::get<Container<Type>>(AsTuple());
		}

		template<typename Type, typename = TypeCondition<Type>>
		constexpr Container<Type>& Get() noexcept
		{
			return std::get<Container<Type>>(AsTuple());
		}

		template<size_t index, typename = IndexCondition<index>>
		constexpr auto const& Get() const noexcept
		{
			return std::get<index>(AsTuple());
		}

		template<size_t index, typename = IndexCondition<index>>
		constexpr auto& Get() noexcept
		{
			return std::get<index>(AsTuple());
		}

		template<typename Callable>
		void ForEach(Callable const& callable)
		{
			(
				([&callable](auto& vec) -> void
				{
					for (auto& obj : vec)
						callable(obj);
				})
				(std::get<Container<Contained>>(AsTuple()))
				, ...
			);
		}

		template<typename Callable>
		void ForEach(Callable const& callable) const
		{
			(
				([&callable] (auto const& vec) -> void
				{
					for (auto const& obj : vec)
						callable(obj);
				})
				(std::get<Container<Contained>>(AsTuple()))
				, ...
			);
		}

		template<typename Callable>
		bool AnyOf(Callable const& callable) const
		{
			return 
			(
				([&callable](auto const& vec) -> bool
					{
						for (auto const& obj : vec)
							if (callable(obj)) return true;
						return false;
					})
				(std::get<Container<Contained>>(AsTuple()))
				|| ...
			);
		}

		size_t size() const noexcept
		{
			constexpr size_t contained = sizeof...(Contained);
			if constexpr (contained == 0)
				return size_t();
			else
			if constexpr (contained == 1)
				return std::size(std::get<0>(AsTuple()));
			else
				return (std::size(std::get<Container<Contained>>(AsTuple())) + ...);
		}

		constexpr Tuple & AsTuple() noexcept
		{
			return *this;
		}
		constexpr Tuple const& AsTuple() const noexcept
		{
			return *this;
		}

	};

};