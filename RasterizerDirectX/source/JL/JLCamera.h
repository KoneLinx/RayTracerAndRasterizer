#pragma once

#include "JLBaseIncludes.h"

namespace JL
{

	template <int N, typename T>
	class Camera
	{
		//constexpr static int N = 3;
		//using T = float;
	public:

		using Value = T;
		using Point = Vector<N, T>;
		using Vector = Vector<N, T>;
		using Axes = Matrix<N, N, T>;
		using Transformation = Matrix<N + 1, N + 1, T>;

		constexpr static Value FRUSTUM_NEAR = 0;
		constexpr static Value FRUSTUM_FAR = 1;
		constexpr static Value VIEW_NEAR = .1f;
		constexpr static Value VIEW_FAR = 100.f;

		static inline const Axes ROOT{ Axes::Identity() };

		Camera()
			: Camera{ Point{}, ROOT[2] }
		{}

		Camera(const Point& position, const Vector& direction)
			: m_Position{ position }
			, m_Direction{ GetNormalized(direction) }
			, m_ScreenAspectRatio{ static_cast<Value>(1) }
			, m_FieldOfView{ static_cast<Value>(70.0 * (M_PI / 180.0)) }
			, m_FocalLength{ static_cast<Value>(1) }
			, m_CameraTransformation{}
			, m_ViewTransformation{}
			, m_ViewRotation{}
			, m_ProjectionTransformation{ CreateProjection() }
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

		const Axes& GetViewRotation() const
		{
			return m_ViewRotation;
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

		Transformation const& GetViewTransformation() const
		{
			return m_ViewTransformation;
		}

		Transformation const& GetProjectionTransformation() const
		{
			return m_ProjectionTransformation;
		}

		template <int D, template <int, typename ...> typename Object>
		Object<D, Value>& ViewToWorld(Object<D, Value>& view) const
		{
			view *= m_CameraTransformation;
			return view;
		}

		template <int D, template <int, typename ...> typename Object>
		Object<D, Value>& WorldToViewVector(Object<D, Value>& world) const
		{
			world *= m_ViewTransformation;
			return world;
		}

		template <int D, template <int, typename ...> typename Object>
		Object<D, Value>& WorldToViewPoint(Object<D, Value>& world) const
		{
			reinterpret_cast<Elite::Point<D, Value>&>(world) *= m_ViewTransformation;
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

		void TransformCamera(const Matrix<N, N, Value>& transformation, Point const& pivot)
		{
			Vector const v{ pivot };
			m_Position -= v;
			reinterpret_cast<Elite::Point<N, T>&>(m_Position) *= transformation;
			m_Position += v;
			TransformCamera(transformation);
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

			m_ViewRotation = Inverse(Axes{ x, y, z });

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

			m_ViewTransformation = m_ProjectionTransformation * Inverse(m_CameraTransformation);

		}

		Transformation CreateProjection() noexcept
		{
			constexpr Value NF{ FRUSTUM_NEAR - FRUSTUM_FAR };
			constexpr Value fn{ VIEW_FAR - VIEW_NEAR };
			constexpr Value f{ NF / fn };
			constexpr Value factor{ f * VIEW_FAR };

			constexpr Value a{ FRUSTUM_NEAR - factor };
			constexpr Value b{ factor * VIEW_NEAR };

			auto projection = Transformation::Identity();
			projection[2].z = a;
			projection[3].z = b;
			projection[2].w = 1;
			projection[3].w = 0;

			return projection;
		}

		Point  m_Position;
		Point  m_RayOrigin;
		Vector m_Direction;

		Value m_ScreenAspectRatio;

		Value m_FieldOfView;
		Value m_FocalLength;

		Transformation m_CameraTransformation;
		Transformation m_ViewTransformation;
		Transformation const m_ProjectionTransformation;
		Axes m_ViewRotation;

	};

};