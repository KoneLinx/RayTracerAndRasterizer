// Camera.h - Camera class.

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
#include "JLCamera.h"
#include "JLRay.h"

namespace JL
{
	/*
	template<int N, typename T>
	class RayCamera final : public Camera<N, T>
	{
		using Camera = Camera<N, T>;

		using typename Camera::Value;
		using typename Camera::Point;
		using typename Camera::Vector;
		using typename Camera::Axes;
		using typename Camera::Transformation;

		using Ray = Ray<N, T>;

	public:

		RayCamera() = default;

		RayCamera(const Point & position, const Vector & direction)
			: Camera{ position, direction }
		{}
		
	};
	*/
}