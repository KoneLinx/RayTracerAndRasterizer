// Visitor.h - provides visitor structure for lambdas

#pragma once

namespace JL
{

	// Structure for combined lambdas
	// constexpr compatible

	template<typename ...Base>
	struct Visitor : Base...
	{
		using Base::operator()...;
	};

	template<typename ...T>
	Visitor(T...)->Visitor<T...>;

	// Usage example:
	/*

	Visitor v{
		[i = 0] (int) mutable { ... },
		[] (float) -> string { ... },
		[&var] (auto) { ... }
	}

	v(5);
	v(7.f);
	v(v);

	*/

};
