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
#include <functional>

namespace JL
{

	template<int N, typename T>
	class Camera
	{

	public:

		using Value = T;
		using Point = Point<N, T>;
		using Vector = Vector<N, T>;
		using Axes = Matrix<N, N, T>;
		using Transformation = Matrix<N + 1, N + 1, T>;

	private:

		Point  m_Position;
		Point  m_RayOrigin;
		Vector m_Direction;
		
		Value m_ScreenAspectRatio;

		Value m_FieldOfView;
		Value m_FocalLength;

		Transformation m_CameraTransformation;
		Transformation m_ViewTransformation;

	public:

		static inline const Axes ROOT{ Axes::Identity() };

		Camera()
			: Camera{ Point{}, ROOT[2] }
		{}

		Camera(const Point& position, const Vector& direction)
			: m_Position{ position }
			, m_Direction{ direction }
			, m_ScreenAspectRatio{ static_cast<Value>(1) }
			, m_FieldOfView{ static_cast<Value>(70.0 * E_TO_RADIANS) }
			, m_FocalLength{ static_cast<Value>(1) }
			, m_CameraTransformation{}
			, m_ViewTransformation{}
		{
			UpdateTransformation();
		}

		const Value& GetFieldOfView() const
		{
			return m_FieldOfView;
		}

		void SetFieldOfView(const Value& fieldOfView)
		{
			m_FieldOfView = fieldOfView;
			UpdateTransformation();
		}

		const Value& GetFocalLength() const
		{
			return m_FocalLength;
		}

		void SetFocalLength(const Value& focalLength)
		{
			m_FocalLength = focalLength;
			UpdateTransformation();
		}

		const Point& GetPosition() const
		{
			return m_Position;
		}

		const Point& GetRayOrigin() const
		{
			return m_RayOrigin;
		}

		void SetPosition(const Point& position)
		{
			m_Position = position;
			UpdateTransformation();
		}

		const Vector& GetDirection() const
		{
			return m_Direction;
		}

		void SetDirection(const Vector& direction)
		{
			m_Direction = direction;
			m_Direction /= Magnitude(m_Direction);
			UpdateTransformation();
		}

		const Value& GetScreenAspectRatio() const
		{
			return m_ScreenAspectRatio;
		}

		void SetScreenAspectRatio(const Value& ratio)
		{
			m_ScreenAspectRatio = ratio;
		}

		void SetScreenAspectRatio(const Value& width, const Value& height)
		{
			m_ScreenAspectRatio = width / height;
		}

		template <int D, template <int, typename ...> typename Object>
		Object<D, Value>& ViewToWorld(Object<D, Value>& view) const
		{
			view *= m_CameraTransformation;
			return view;
		}

		template <int D, template <int, typename ...> typename Object>
		Object<D, Value>& WorldToView(Object<D, Value>& world) const
		{
			world *= m_ViewTransformation;
			return world;
		}

		void TransformCamera(const Matrix<N, N, Value>& transformation)
		{
			m_Position -= m_Direction;
			m_Direction *= transformation;
			m_Direction /= Magnitude(m_Direction);
			m_Position += m_Direction;
			UpdateTransformation();
		}

		void TransformCamera(const Matrix<N + 1, N + 1, Value>& transformation)
		{
			m_Position -= m_Direction;
			m_Direction *= transformation;
			m_Direction /= Magnitude(m_Direction);
			m_Position += Vector{ transformation[4] };
			m_Position += m_Direction;
			UpdateTransformation();
		}

	private:

		void UpdateTransformation()
		{

			const Value fov{ tan(m_FieldOfView / static_cast<Value>(2)) };
			const Value ratio{ static_cast<Value>(sqrt(m_ScreenAspectRatio)) };
			const Value xf{ fov * ratio };
			const Value yf{ fov * (static_cast<Value>(1) / ratio) };
			const Value zf{ m_FocalLength };

			const Vector& z{ m_Direction };
			const Vector& Y{ ROOT[1] };
			const Vector  x{ GetNormalized(Cross(Y, z)) };
			const Vector  y{ Cross(z, x) };

			const Axes  xyz{
				x * xf,
				y * yf,
				z * zf
			};

			m_RayOrigin = m_Position + xyz * Vector{ 0, 0, -1 };

			const Vector  t{ m_RayOrigin };

			m_CameraTransformation = Transformation{
				xyz, t
			};

			m_ViewTransformation = Transformation{
				Inverse(xyz), -t
			};


		}

	};

}