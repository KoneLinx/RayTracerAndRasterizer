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
#include <iostream>
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

		struct Face
		{
			static constexpr char NAME[2]{ "f" };
			static constexpr unsigned SIZE = 3;
			using type = Vertex*;
			union
			{
				type data[SIZE];
				type vertices[SIZE];
				struct { type a, b, c; };
			};
		};

		std::vector<Vertex> vertices;
		std::vector<Normal> normals;
		std::vector<Face> faces;

		OBJ(OBJ const&) = delete;
		OBJ(OBJ&&) = delete;
		OBJ& operator = (OBJ const&) = delete;
		OBJ& operator = (OBJ&&) = delete;
	};

	template<typename Char, typename Object, typename ReadType = typename Object::type, typename = std::enable_if_t<(sizeof(typename Object::type) >= sizeof(ReadType))>>
	std::basic_istream<Char>& Read(std::basic_istream<Char>& is, Object& object)
	{
		auto read = [&is, &object]()
		{
			if constexpr (!std::is_same<typename Object::type, ReadType>::value)
			{
				object.data[0] = reinterpret_cast<typename Object::type>(Read<ReadType>(is));
				object.data[1] = reinterpret_cast<typename Object::type>(Read<ReadType>(is));
				object.data[2] = reinterpret_cast<typename Object::type>(Read<ReadType>(is));
			}
			else
			{
				Read<ReadType>(is, object.data[0]);
				Read<ReadType>(is, object.data[1]);
				Read<ReadType>(is, object.data[2]);
			}
		};
		try
		{
			read();
		}
		catch (const std::invalid_argument&)
		{
			bool correct{ true };
			for (const char* c{ object.NAME }; *c != '\0'; ++c)
				correct &= (Read<Char>(is) == static_cast<Char>(*c));
			if (correct)
				Read<Char>(is), read();
		}
		return is;
	}

	template<typename Char, typename Object>
	std::basic_ostream<Char>& Write(std::basic_ostream<Char>& os, Object const& object)
	{
		return os
			<< object.NAME << ' '
			<< object.data[0] << ' '
			<< object.data[1] << ' '
			<< object.data[2] << std::endl;
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::Vertex& vertex)
	{
		return Read(is, vertex);
	}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, OBJ::Vertex const& vertex)
	{
		return Write(os, vertex);
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::Normal& normal)
	{
		return Read(is, normal);
	}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, OBJ::Normal const& normal)
	{
		return Write(os, normal);
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ::Face& face)
	{
		return Read<Char, OBJ::Face, int>(is, face);
	}

	template<typename = void>
	auto AsEntry(OBJ const& obj, OBJ::Face const& face)
	{
		const void* zeroIndex{ obj.vertices.data() - 1 };
		struct FaceEntry
		{
			int data[3];
			const char(&NAME)[std::size(OBJ::Face::NAME)]{ OBJ::Face::NAME };
		};
		return FaceEntry{
			static_cast<int>(face.a - zeroIndex),
			static_cast<int>(face.b - zeroIndex),
			static_cast<int>(face.c - zeroIndex)
		};
	}

	template<typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, std::invoke_result_t<decltype(AsEntry<>), OBJ&, OBJ::Face&> const& faceEntry)
	{
		return Write(os, faceEntry);
	}

	template<typename Size = int>
	void ValidateFace(OBJ& obj, OBJ::Face& face)
	{
		face.data[0] = &obj.vertices.at(reinterpret_cast<Size>(face.data[0]) - 1);
		face.data[1] = &obj.vertices.at(reinterpret_cast<Size>(face.data[1]) - 1);
		face.data[2] = &obj.vertices.at(reinterpret_cast<Size>(face.data[2]) - 1);
	}

	template<typename Char>
	std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, OBJ& obj)
	{
		while (is)
		{
			constexpr size_t SIZE{ 3 };
			Char str[SIZE + 1]{};
			is.getline(str, SIZE, ' ');
			std::basic_string<Char> comment{};
			if (std::strcmp(str, OBJ::Vertex::NAME) == 0)
			{
				obj.vertices.push_back({});
				is >> obj.vertices.back();
			}
			else if (std::strcmp(str, OBJ::Normal::NAME) == 0)
			{
				obj.normals.push_back({});
				is >> obj.normals.back();
				//is.get(*str);
			}
			else if (std::strcmp(str, OBJ::Face::NAME) == 0)
			{
				obj.faces.push_back({});
				is >> obj.faces.back();
				//is.get(*str);
			}
			std::getline(is, comment, '\n');
		}
		for (OBJ::Face& face : obj.faces)
			ValidateFace<int>(obj, face);
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
			os << AsEntry(obj, face);
		return os;
	}

}