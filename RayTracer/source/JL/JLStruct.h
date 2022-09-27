#pragma once

namespace JL
{

	template<typename ... Objects>
	struct Struct : Objects...
	{

		using Objects::Objects...;

		Struct(Objects const&... objects)
			: Objects{ objects }...
		{}

	};

}