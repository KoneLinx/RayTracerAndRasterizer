// Mesh.h - mesh structure.

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

#include "JLBaseIncludes.h"
#include "JLOBJ.h"
#include <vector>

namespace JL
{

	template<typename Vertex_t, typename Index_t, typename Point_t = Vertex_t>
	struct Mesh
	{

		using Index = Index_t;
		using Vertex = Vertex_t;
		using Point = Point_t;

		struct Face
		{
			union
			{
				struct
				{
					Index x, z, y;
				};
				Index data[3];
			};
			constexpr static size_t SIZE = 3;
		};

		template<typename ... T>
		using Container = std::vector<T...>;

		Container<Vertex> vertices;
		Container<Face> faces;
		Point center = {};

	};

	//template<typename Vertex_t, typename UV_t, typename Normal_t, template<typename ...> typename Container_t = std::vector>
	//struct Mesh2
	//{

	//	using Index  = unsigned;
	//	using Vertex = Vertex_t;
	//	using UV     = UV_t;
	//	using Normal = Normal_t;

	//	struct FaceIndex
	//	{
	//		Index v;
	//		Index t;
	//		Index n;
	//		//Index tangent;
	//	};

	//	using Face = JL::Point<3, FaceIndex>;

	//	template<typename ... T>
	//	using Container = Container_t<T...>;

	//	Container<Vertex> vertices;
	//	Container<UV> uvs;
	//	Container<Normal> normals;
	//	//Container<Normal> tangents;
	//	Container<Face> faces;

	//	Vertex center;

	//};

}