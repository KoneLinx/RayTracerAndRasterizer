// OBJ.h - provides .obj file parsing functionallity

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
#include <fstream>
#include <utility>
#include "JLReadFromIstream.h"

namespace JL
{

	struct OBJ
	{
		OBJ() = default;

		struct Vertex
		{
			static constexpr char NAME[2]{ "v" };
			static constexpr unsigned SIZE = 3;
			using type = float;
			union
			{
				type data[SIZE];
				type values[SIZE];
				struct { type x, y, z; };
			};
		};

		struct Normal : Vertex
		{
			static constexpr char NAME[3]{ "vn" };
		};

		struct UVW : Vertex
		{
			static constexpr char NAME[3]{ "vt" };
		};

		struct Face
		{
			static constexpr char NAME[2]{ "f" };
			static constexpr unsigned SIZE = 9;
			
			union vertex {
				using type = unsigned;
				type data[3];
				struct {
					type v, t, n;
				};
			};

			union {
				vertex data[4];
				struct {
					vertex x, y, z, w;
				};
			};

			bool quad = false;
		};

		std::vector<Vertex> vertices;
		std::vector<Normal> normals;
		std::vector<UVW> uvws;
		std::vector<Face> faces;

		OBJ(OBJ const&) = delete;
		OBJ(OBJ&&) = default;
		OBJ& operator = (OBJ const&) = delete;
		OBJ& operator = (OBJ&&) = default;
	};

	template<typename Char, typename Object, typename ReadType = typename Object::type, typename = std::enable_if_t<(sizeof(typename Object::type) >= sizeof(ReadType))>>
	std::basic_istream<Char>& ReadObject(std::basic_istream<Char>& is, Object& object)
	{
		char c;

		Read<ReadType>(is, object.data[0]);

		if (is.peek() == '\n')
			return is;
		is.get(c);
		if (is.peek() == '\n')
			return is;

		Read<ReadType>(is, object.data[1]);

		if (is.peek() == '\n')
			return is;
		is.get(c);
		if (is.peek() == '\n')
			return is;

		Read<ReadType>(is, object.data[2]);

		return is;
	}

	template<typename Char, typename Object>
	std::basic_ostream<Char>& Write(std::basic_ostream<Char>& os, Object const& object)
	{
		return os
			<< object.NAME    << " "
			<< object.data[0] << " "
			<< object.data[1] << " "
			<< object.data[2] << "\n";
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::Vertex& vertex)
	{
		ReadObject(is, vertex);
		vertex.z *= -1;
		return is;
	}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, OBJ::Vertex const& vertex)
	{
		return Write(os, vertex);
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::UVW& uvw)
	{
		ReadObject(is, uvw);
		uvw.y = 1 - uvw.y;
		return is;
	}

	//template<typename Char>
	//std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::Normal& normal)
	//{
	//	return ReadObject(is, normal);
	//}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, OBJ::Normal const& normal)
	{
		return Write(os, normal);
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::Face& face)
	{
		ReadObject(is, face.x);
		--face.x.v; --face.x.n; --face.x.t;
		ReadObject(is, face.y);
		--face.y.v; --face.y.n; --face.y.t;
		ReadObject(is, face.z);
		--face.z.v; --face.z.n; --face.z.t;
		
		char c;
		if (is.peek() == '\n')
			return is;
		is.get(c);
		if (is.peek() == '\n')
			return is;
		
		face.quad = true;
		ReadObject(is, face.w);
		--face.w.v; --face.w.n; --face.w.t;

		return is;
	}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, OBJ::Face const& face)
	{
		return Write(os, face);
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ& obj)
	{
		while (is)
		{

			constexpr size_t SIZE{ 16 };
			Char str[SIZE + 1]{};

			is.get(*str);
			if (*str != '\n')
			{
				if (*str != '#')
				{

					is.getline(str + 1, SIZE - 1, ' ');

					if (std::strcmp(str, OBJ::Vertex::NAME) == 0)
					{
						obj.vertices.push_back({});
						is >> obj.vertices.back();
					}
					else if (std::strcmp(str, OBJ::Normal::NAME) == 0)
					{
						obj.normals.push_back({});
						is >> obj.normals.back();
					}
					else if (std::strcmp(str, OBJ::UVW::NAME) == 0)
					{
						obj.uvws.push_back({});
						is >> obj.uvws.back();
					}
					else if (std::strcmp(str, OBJ::Face::NAME) == 0)
					{
						obj.faces.push_back({});
						is >> obj.faces.back();
					}
				
				}

				if (is.peek() != '\n')
				{
					std::string comment{};
					std::getline(is, comment);
				}

			}
		}
		
		return is;
	}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, OBJ const& obj)
	{
		for (OBJ::Vertex const& vertex : obj.vertices)
			os << vertex;
		for (OBJ::Normal const& normal : obj.normals)
			os << normal;
		for (OBJ::Face const& face : obj.faces)
			os << face;
		return os;
	}

	template<typename = void>
	OBJ OBJ_Load(char const* file)
	{
		OBJ obj{};
		std::ifstream{ file } >> obj;
		return obj;
	}

}