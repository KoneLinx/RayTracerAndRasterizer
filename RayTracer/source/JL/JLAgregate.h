// Agregate.h - provides agregate structure

#pragma once

#include <utility>

namespace JL
{

	template<typename One, typename ... Many>
	using is_any_of = std::bool_constant<(std::is_same_v<One, Many> || ...)>;

	template<typename One, typename ... Many>
	constexpr bool is_any_of_v = is_any_of<One, Many...>::value;

	template<size_t index, typename ... Types>
	using get = std::tuple_element_t<index, std::tuple<Types...>>;

	template<typename ... Bases>
	struct Agregate : public Bases...
	{

		Agregate() = default;

		Agregate(Bases const& ... bases)
			: Bases{ bases } ...
		{}

		Agregate(Bases && ... bases)
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
			return As<get<index, Bases...>>();
		}

		template<size_t index>
		constexpr auto const& Get() const noexcept
		{
			return As<get<index, Bases...>>();
		}
		
		template<typename T>
		static constexpr bool Has() noexcept
		{
			return is_any_of_v<T, Bases...>;
		}

	};

}