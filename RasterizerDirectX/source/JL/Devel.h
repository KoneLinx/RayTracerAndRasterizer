// Devel.h - General functions that don't have a place yet.

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
#include "JL.h"
#include "../RenderUtils.h"
#include <map>

namespace Elite
{

	template<typename = void>
	Projection Project(Projection const& object)
	{
		return {
			object.x / -object.w,
			object.y / -object.w,
			object.z / -object.w,
		};
	}

	template<typename = void>
	Projection& Project(Projection& object)
	{
		WorldValue const n = object.w;
		object.x /= n;
		object.y /= n;
		object.z /= n;
		return object;
	}

	template<template<int, typename> typename Object>
	Object<3, Projection>& ProjectPoints(Object<3, Projection>& points)
	{
		Project(points.x);
		Project(points.y);
		Project(points.z);
		return points;
	}

	template<typename ... T, template<typename ...> typename Container>
	Container<Projection, T...>& ProjectPoints(Container<Projection, T...>& points)
	{
		for (auto& point : points)
			Project(point);
		return points;
	}

	template<typename T, template<int, typename> typename Object>
	auto Sum(Object<3, T> const& obj)
	{
		return obj.x + obj.y + obj.z;
	}


	template<bool backface = false>
	auto PointInTriangle(RasterPoint const& point, Face const& triangle)
		-> std::pair<bool, Vector<3, WorldValue>>
	{

		constexpr auto check =
			[](RasterPoint const& p, Projection const& a, Projection const& b, WorldValue& area)
			{
				using Vector = Vector<2, WorldValue>;
				Vector const toPoint{ p.x - a.x, p.y - a.y };
				Vector const side{ b.x - a.x, b.y - a.y };
				area = Cross(toPoint, side);
				if constexpr (backface)
					return area < 0;
				else
					return area > 0;
			};

		Vector<3, WorldValue> result{};

		if (check(point, *triangle.x, *triangle.y, result.z) &&
			check(point, *triangle.y, *triangle.z, result.x) &&
			check(point, *triangle.z, *triangle.x, result.y) )
				return { true, result /= Sum(result) };
		
		return { false, {} };
	}
	
	template<typename T>
	T Invert(T const& val)
	{
		return 1 / val;
	}

	template<typename T, template<int, typename> typename Object>
	Object<3, T> Invert(Object<3, T> const& object)
	{
		return {
			Invert(object.x),
			Invert(object.y),
			Invert(object.z)
		};
	}
	
	template<typename T, typename R, template<int, typename> typename Object1, template<int, typename> typename Object2>
	Object1<3, T>& Divide(Object1<3, T>& numerator, Object2<3, R> const& denominator)
	{
		numerator.x /= denominator.x;
		numerator.y /= denominator.y;
		numerator.z /= denominator.z;
		return numerator;
	}

	template<typename T, template<int, typename> typename Object>
	Object<3, T>& GetDivided(Object<3, T> numerator, Object<3, T> const& denominator)
	{
		return Devide(numerator, denominator);
	}

	template<int N, typename T>
	Matrix<N, N, T> GetNormalTransformation(Matrix<N, N, T> const& transformation)
	{
		return Transpose(Inverse(transformation));
	}

	template<typename = void>
	WorldValue DepthInterpolation(Vector<3, WorldValue> positionResult, Vector<3, WorldValue> const& depths)
	{
		return Invert(Sum(Divide(positionResult, depths)));
	}

	template<typename = void>
	auto DepthInterpolation(Vector<3, WorldValue> const& positionResult, Vector<3, Colour> colour)
	{
		return Sum(JL::Scale(colour, positionResult));
	}

	template<typename T>
	auto DepthInterpolation(Vector<3, WorldValue> const& positionResult, Vector<3, WorldValue> const& depths, Vector<3, T> values)
	{
		return Sum(JL::Scale(Divide(values, depths), positionResult));
	}

	template<typename = void>
	Colour UVToColour(UVPoint const& uv)
	{
		return {
			uv.x,
			uv.y,
			JL::Min(uv.x, uv.y)
		};
	}

	template<typename T, template<int, typename> typename Object>
	Object<3, T>& TransformToView(Object<3, T>& points, Camera const& camera)
	{
		camera.WorldToViewPoint(points.x);
		camera.WorldToViewPoint(points.y);
		camera.WorldToViewPoint(points.z);
		return points;
	}

	template<typename ... T, template<typename ...> typename Container>
	Container<Projection, T...>& TransformToView(Container<Projection, T...>& points, Camera const& camera)
	{
		for (auto& point : points)
			camera.WorldToViewPoint(point);
		return points;
	}

	template<template<int, typename> typename Object>
	Object<3, Projection>& PointsToRaster(Object<3, Projection>& triangle, RasterValue const width, RasterValue const height)
	{
		PointToRaster(triangle.x, width, height);
		PointToRaster(triangle.y, width, height);
		PointToRaster(triangle.z, width, height);
		return triangle;
	}

	template<typename ...T, template<typename ...> typename Container>
	Container<Projection, T...>& PointsToRaster(Container<Projection, T...>& points, RasterValue const width, RasterValue const height)
	{
		for(auto & point : points)
			PointToRaster(point, width, height);
		return points;
	}

	template<typename = void>
	RasterBoundingBox GetRasterBoundingBox(Face const& object, RasterValue const width, RasterValue const height)
	{
		return {
			/*RasterBoundingBox::first_type const tl*/{
				static_cast<RasterValue>(/*JL::Max<int>(0         ,*/ JL::Min<int>(int(object.x->x), int(object.y->x), int(object.z->x) /*, width  - 1)*/)),
				static_cast<RasterValue>(/*JL::Max<int>(0         ,*/ JL::Min<int>(int(object.x->y), int(object.y->y), int(object.z->y) /*, height - 1)*/))
			},
			/*RasterBoundingBox::second_type const br*/{
				static_cast<RasterValue>(/*JL::Min<int>(width  - 1,*/ JL::Max<int>(int(object.x->x), int(object.y->x), int(object.z->x) /*, 0         )*/)),
				static_cast<RasterValue>(/*JL::Min<int>(height - 1,*/ JL::Max<int>(int(object.x->y), int(object.y->y), int(object.z->y) /*, 0         )*/))
			}
		};
		//if (tl.x > br.x || tl.y > br.y)
		//	return { {}, {} };
		//return { tl, br };
	}

	template<typename R, typename T, template<typename ...> typename Container>
	Container<R> CopyVertices(Container<T> const& vertices)
	{
		size_t const size{ vertices.size() };
		Container<R> out{ size };
		for (size_t i{}; i < size; ++i)
			if constexpr (std::is_same_v<List::Vertex, T>)
				out[i] = { vertices[i].pos, 1.f };
			else
				out[i] = { vertices[i], 1.f };

		return out;
	}

	template<typename = void>
	WorldPoint CalculateCenter(Triangle const& triangle)
	{
		return {
			(triangle.x.x + triangle.y.x + triangle.z.x) / 3,
			(triangle.x.y + triangle.y.y + triangle.z.y) / 3,
			(triangle.x.z + triangle.y.z + triangle.z.z) / 3
		};
	}

	template<typename ...T, template<typename ...> typename Container>
	WorldPoint CalculateCenter(Container<T...> const& points)
	{
		WorldPoint out{};
		for (auto const& point : points)
		{
			if constexpr(std::is_same_v<decltype(point), List::Vertex const&>)
				out += point.pos;
			else
				out += point;
		}
		auto const size = points.size();
		out /= size;
		return out;
	}

	template<typename = void>
	WorldPoint CalculateCenter(Scene const& scene)
	{
		std::vector<WorldPoint> points{};
		points.reserve(scene.objects.size());
		
		scene.objects.ForEach( JL::Visitor{

				[&points](Object<Triangle> const& object)
				{
					points.push_back(CalculateCenter(object.As<Triangle>()));
				},

				//[&points](Object<Mesh> const& object)
				//{
				//	points.push_back(object.center);
				//},

				[&points](auto const& object)
				{
					points.push_back(CalculateCenter(object.vertices));
				}

		});

		return CalculateCenter(points);
	}

	template<int N, typename T>
	auto& Transform(Triangle& triangle, Matrix<N, N, T> const& transformation)
	{
		triangle.x *= transformation;
		triangle.y *= transformation;
		triangle.z *= transformation;
		return triangle;
	}

	template<int N, typename T>
	auto& Transform(Triangle& triangle, Matrix<N, N, T> const& transformation, WorldPoint const& center)
	{
		WorldVector const v{ center };
		((triangle.x -= v) *= transformation) += v;
		((triangle.y -= v) *= transformation) += v;
		((triangle.z -= v) *= transformation) += v;
		return triangle;
	}

	template<int N, typename M, typename ...T, template<typename ...> typename Container>
	auto& Transform(Container<T...>& points, Matrix<N, N, M> const& transformation)
	{
		for (WorldPoint & point : points)
			point *= transformation;
		return points;
	}

	template<int N, typename M, typename ...T, template<typename ...> typename Container>
	auto& Transform(Container<T...>& points, Matrix<N, N, M> const& transformation, WorldPoint const& center)
	{
		WorldVector const v{ center };
		for (WorldPoint& point : points)
			((point -= v) *= transformation) += v;
		return points;
	}

	//template<int N, typename M>
	//auto& Transform(Mesh& mesh, Matrix<N, N, M> const& transformation, WorldPoint const& center)
	//{
	//	Transform(mesh.vertices, transformation, center);
	//	Transform(mesh.normals, GetNormalTransformation(transformation));
	//	return mesh;
	//}

	//template<int N, typename M>
	//auto& Transform(Mesh& mesh, Matrix<N, N, M> const& transformation)
	//{
	//	Transform(mesh.vertices, transformation, mesh.center);
	//	return mesh;
	//}

	template<int N, typename M>
	auto& Transform(List& list, Matrix<N, N, M> const& transformation)
	{
		Transform(list.vertices, transformation, list.center);
		return list;
	}

	template<typename ...T, template<typename ...> typename Container>
	auto& Translate(Container<T...>& points, WorldVector const& v)
	{
		for (auto& point : points)
			point += v;
		return points;
	}

	//template<typename = void>
	//auto& Translate(Mesh& mesh, WorldVector const& v)
	//{
	//	Translate(mesh.vertices, v);
	//	mesh.center += v;
	//	return mesh;
	//}

	//template<typename = void>
	//auto& Translate(List& list, WorldVector const& v)
	//{
	//	Translate(list.vertices, v);
	//	list.center += v;
	//	return list;
	//}

	template<typename = void>
	WorldVector GetTangent(	WorldPoint const& xv, WorldPoint const& yv, WorldPoint const& zv,
							UVPoint    const& xt, UVPoint    const& yt, UVPoint    const& zt	)
	{
		auto const edge1 = yv - xv;
		auto const edge2 = zv - xv;
		UVPoint const dx{ yt.x - xt.x, zt.x - xt.x };
		UVPoint const dy{ yt.y - xt.y, zt.y - xt.y };
		auto const r = Cross(dx, dy);
		return { edge1 * (dy.y / r) - edge2 * (dy.x / r) };
	}

	template<bool center = true>
	List::Mesh ConstructMesh(JL::OBJ const& obj)
	{
		using Vertex = List::Vertex;
		using Mesh   = List::Mesh;

		Mesh list{};

		list.faces.reserve(
			obj.faces.size()
			+ std::count_if(
				begin(obj.faces),
				end(obj.faces),
				JL::MemberGetter(&JL::OBJ::Face::quad)
			)
		);

		// Hoping only few meshes will exceed this number. Shrinking at the bottom.
		list.vertices.reserve(list.faces.capacity() * 2);

		auto addVertex =
			[&list, map = std::map<Vertex const*, List::Index, JL::PtrCmp<Vertex, JL::LessMemcmp>>{}]
			(Vertex vertex) mutable -> List::Index
			{
				auto const result = map.find(&vertex);
				if (result != end(map))
				// Vertex present
				{
					return result->second;
				}
				// vertex not present
				else
				{
					auto const index = list.vertices.size();
					list.vertices.push_back(std::move(vertex));
					map.emplace(&list.vertices.back(), index);
					return index;
				}
			};

		for (auto const& face : obj.faces)
		{
			List::Index vertices[4]{};
			size_t const end = face.quad ? 4 : 3;
			for (size_t i{}; i < end; ++i)
			{
				auto const& v = obj.vertices[face.data[i].v];
				auto const& t = obj.uvws[face.data[i].t];
				auto const& n = obj.normals[face.data[i].n];
				vertices[i] = addVertex(
					{
						{ v.x, v.y, v.z },
						{ t.x, t.y },
						{ n.x, n.y, n.z }
					}
				);
			}
			list.faces.push_back({ vertices[0], vertices[2], vertices[1] });	 // Swapped Y and Z !!
			if (face.quad)
				list.faces.push_back({ vertices[0], vertices[3], vertices[2] }); // Swapped Y and Z !!
		}

		list.vertices.shrink_to_fit();

		for (auto & face : list.faces)
		{
			auto& x = list.vertices[face.x];
			auto& y = list.vertices[face.y];
			auto& z = list.vertices[face.z];
			auto const tangent = GetTangent(
				x.pos, y.pos, z.pos,
				x.uv , y.uv , z.uv
			);
			x.tangent += tangent;
			y.tangent += tangent;
			z.tangent += tangent;
		}
		for (auto& vertex : list.vertices)
			static_cast<WorldVector&>(vertex.tangent) = GetNormalized(Reject(vertex.tangent, vertex.normal));

		//list.center = CalculateCenter(list.vertices);

		//if constexpr (center)
		//	Translate(list, WorldPoint{} - list.center);

		return list;
	}

	//template<bool center = true>
	//Mesh ConstructMesh(JL::OBJ const& obj)
	//{

	//	Mesh mesh{};
	//	mesh.vertices.reserve(obj.vertices.size());
	//	mesh.uvs.reserve(obj.uvws.size());
	//	mesh.normals.reserve(obj.normals.size());
	//	//mesh.tangents.reserve(obj.vertices.size());
	//	mesh.faces.reserve(obj.faces.size()
	//		+ std::count_if(
	//			begin(obj.faces),
	//			end(obj.faces),
	//			JL::MemberGetter(&JL::OBJ::Face::quad)
	//		)
	//	);

	//	for (auto const& v : obj.vertices)
	//		mesh.vertices.emplace_back(v.x, v.y, v.z);

	//	for (auto const& u : obj.uvws)
	//		mesh.uvs.emplace_back(u.x, u.y);

	//	for (auto const& n : obj.normals)
	//		mesh.normals.emplace_back(n.x, n.y, n.z);

	//	for (auto const& f : obj.faces)
	//	{
	//		//auto const tangent{ GetTangent(
	//		//	mesh.vertices[f.x.v], mesh.vertices[f.y.v], mesh.vertices[f.z.v],
	//		//	mesh.uvs[f.x.t], mesh.uvs[f.y.t], mesh.uvs[f.z.t]
	//		//) };
	//		mesh.faces.emplace_back(
	//			/*(mesh.tangents[f.x.v] += tangent,*/ Mesh::FaceIndex{ f.x.v, f.x.t, f.x.n }/*)*/,
	//			/*(mesh.tangents[f.y.v] += tangent,*/ Mesh::FaceIndex{ f.y.v, f.y.t, f.y.n }/*)*/,
	//			/*(mesh.tangents[f.z.v] += tangent,*/ Mesh::FaceIndex{ f.z.v, f.z.t, f.z.n }/*)*/
	//		);
	//		if (f.quad)
	//		{
	//			//auto const tangent{ GetTangent(
	//			//	mesh.vertices[f.x.v], mesh.vertices[f.z.v], mesh.vertices[f.w.v],
	//			//	mesh.uvs     [f.x.t], mesh.uvs     [f.z.t], mesh.uvs     [f.w.t]
	//			//) };
	//			mesh.faces.emplace_back(
	//				/*(mesh.tangents[f.x.v] += tangent,*/ Mesh::FaceIndex{ f.x.v, f.x.t, f.x.n }/*)*/,
	//				/*(mesh.tangents[f.z.v] += tangent,*/ Mesh::FaceIndex{ f.z.v, f.z.t, f.y.n }/*)*/,
	//				/*(mesh.tangents[f.w.v] += tangent,*/ Mesh::FaceIndex{ f.w.v, f.w.t, f.z.n }/*)*/
	//			);
	//		}
	//	}

	//	mesh.center = CalculateCenter(mesh.vertices);

	//	if constexpr (center)
	//		Translate(mesh, WorldPoint{} - mesh.center);

	//	return mesh;
	//}

	template<typename T>
	Vector<3, T>& MaxToOne(Vector<3, T>& colour)
	{
		return colour /= JL::Max(colour.r, colour.g, colour.b, 1);
	}

	template<typename = void>
	Colour& Clamp(Colour& colour)
	{
		colour.r = std::clamp(colour.r, 0, 1);
		colour.g = std::clamp(colour.g, 0, 1);
		colour.b = std::clamp(colour.b, 0, 1);
		return colour;
	}

	template<typename = void>
	ColourValue Phong(WorldVector const& view, WorldVector const& lightReflection, ColourValue const& phongExp)
	{
		// Pe: Phong exp. // Ks: Specular reflectance factor
		//  (r*n) ^ Pe * Ks = Phong Intensity
		return std::pow(JL::Max<WorldValue>(0, -Dot(view, lightReflection)), phongExp);
	}

	template<typename ...T, template<typename ...> typename Container>
	Container<T...>& NegateAll(Container<T...>& container)
	{
		for (auto& el : container)
			el = -el;
		return container;
	}

}
