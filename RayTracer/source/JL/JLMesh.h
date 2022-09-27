// Mesh.h - provides Mesh structure

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
#include <vector>
#include <utility>

namespace JL
{

	template<typename Vertex_t, typename Triangle_t>
	struct MeshData
	{
		template<typename T>
		using Container = std::vector<T>;
		using Vertex = Vertex_t;
		using Triangle = Triangle_t;

		Container<Vertex> vertices;
		Container<Triangle> triangles = {};

		Vertex center{};
	};

	template<int N, typename T>
	class Mesh : private MeshData<Point<N, T>, Point<3, Point<N, T> const*>>
	{
	public:

		using MeshData = MeshData<Point<N, T>, Point<3, Point<N, T> const*>>;

		constexpr Mesh() noexcept = default;

		~Mesh() noexcept = default;

		constexpr Mesh(Mesh&&) noexcept = default;
		constexpr Mesh& operator = (Mesh&&) noexcept = default;

		explicit Mesh(Mesh const& other)
			: MeshData{ other.vertices }
		{
			MeshData::center = other.center;
			CopyTriangles(other, std::make_index_sequence<3>{});
		}

		Mesh& operator = (Mesh const& other)
		{
			MeshData::vertices = other.vertices;
			MeshData::center = other.center;
			CopyTriangles(other, std::make_index_sequence<3>{});
		}

		//static constexpr Mesh const& AsMesh(MeshData const& data)
		//{
		//	return static_cast<Mesh const&>(data);
		//}

		//static constexpr Mesh & AsMesh(MeshData & data)
		//{
		//	Mesh& mesh{ data };
		//	mesh.center = mesh.ResetCenter();
		//	return mesh;
		//}

		constexpr MeshData& AsData() noexcept
		{
			return *this;
		}

		constexpr auto const& GetVertices() const noexcept
		{
			return MeshData::vertices;
		}

		constexpr auto const& GetTriangles() const noexcept
		{
			return MeshData::triangles;
		}

		auto begin() const noexcept
		{
			return MeshData::triangles.cbegin();
		}

		auto end() const noexcept
		{
			return MeshData::triangles.cend();
		}

		void Translate(Vector<N, T> const& translation)
		{
			for (auto& vertice : MeshData::vertices)
				vertice += translation;
			MeshData::center += translation;
		}

		void Scale(T const& scale)
		{
			for (auto& vertice : MeshData::vertices)
				vertice *= scale;
			MeshData::center *= scale;
		}

		void Scale(Vector<N, T> const& scale)
		{
			for (auto& vertice : MeshData::vertices)
				JL::Scale(vertice, scale);
			JL::Scale(MeshData::center, scale);
		}

		template<int D> 
		void Transform(Matrix<D, D, T> const& transformation)
		{
			Transform(transformation, MeshData::center);
		}

		template<int D>
		void Transform(Matrix<D, D, T> const& transformation, typename MeshData::Vertex const& pivot)
		{
			Vector<N, T> const move{ pivot };
			for (auto& vertice : MeshData::vertices)
			{
				vertice -= move;
				vertice *= transformation;
				vertice += move;
			}
		}

		void ResetCenter()
		{
			Vector<N, T> pos{};
			for (auto const& vertice : MeshData::vertices)
				pos += Vector<N, T>{ vertice };
			pos *= static_cast<T>(1) / MeshData::vertices.size();
			MeshData::center = typename MeshData::Vertex{ pos };
		}

		void SetCenter(typename MeshData::Vertex const& center)
		{
			MeshData::center = center;
		}

	private:

		template<size_t ... INDECES>
		void CopyTriangles(Mesh const& other, std::index_sequence<INDECES...>)
		{
			ptrdiff_t const diff{ MeshData::vertices.data() - other.vertices.data() };
			for (auto const& triangle : other.triangles)
				MeshData::triangles.emplace_back( (triangle.data[INDECES] + diff) ... );
		}

	};

}