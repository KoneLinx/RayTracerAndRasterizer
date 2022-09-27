#include "RenderUtils.h"

Elite::NDCPoint& Elite::RasterToNCD(NDCPoint& result, const RasterPoint value, const RasterValue width, const RasterValue height)
{
	result.x = static_cast<NDCValue>(value.x) / static_cast<NDCValue>(width);
	result.y = static_cast<NDCValue>(value.y) / static_cast<NDCValue>(width);
	return result;
}

Elite::RasterPoint& Elite::NDCToRaster(RasterPoint& result, const NDCPoint value, const RasterValue width, const RasterValue height)
{
	result.x = static_cast<RasterValue>( static_cast<NDCValue>(value.x) * static_cast<NDCValue>(width) );
	result.y = static_cast<RasterValue>( static_cast<NDCValue>(value.y) * static_cast<NDCValue>(width) );
	return result;
}

Elite::ScreenPoint& Elite::RasterToScreen(ScreenPoint& result, const RasterPoint value, const RasterValue width, const RasterValue height)
{
	result.x = static_cast<ScreenValue>(2) * (static_cast<ScreenValue>(value.x) + static_cast<ScreenValue>(0.5)) / static_cast<ScreenValue>(width ) - static_cast<ScreenValue>(1);
	result.y = static_cast<ScreenValue>(1) - (static_cast<ScreenValue>(value.y) + static_cast<ScreenValue>(0.5)) / static_cast<ScreenValue>(height) * static_cast<ScreenValue>(2);
	return result;
}

Elite::RasterPoint& Elite::ScreenToRaster(RasterPoint& result, const ScreenPoint value, const RasterValue width, const RasterValue height)
{
	result.x = static_cast<RasterValue>( static_cast<ScreenValue>(width ) / static_cast<ScreenValue>(2) * (value.x + static_cast<ScreenValue>(1)) - static_cast<ScreenValue>(0.5) );
	result.x = static_cast<RasterValue>( static_cast<ScreenValue>(height) / static_cast<ScreenValue>(2) * (static_cast<ScreenValue>(1) - value.x) - static_cast<ScreenValue>(0.5) );
	return result;
}

Elite::ScreenPoint Elite::RasterToScreen(const RasterPoint value, const RasterValue width, const RasterValue height)
{
	return ScreenPoint{
		static_cast<ScreenValue>(2) * (static_cast<ScreenValue>(value.x) + static_cast<ScreenValue>(0.5)) / static_cast<ScreenValue>(width ) - static_cast<ScreenValue>(1),
		static_cast<ScreenValue>(1) - (static_cast<ScreenValue>(value.y) + static_cast<ScreenValue>(0.5)) / static_cast<ScreenValue>(height) * static_cast<ScreenValue>(2)
	};
}

Elite::RasterPoint Elite::ScreenToRaster(const ScreenPoint value, const RasterValue width, const RasterValue height)
{
	return RasterPoint{
		static_cast<RasterValue>( static_cast<ScreenValue>(width ) / static_cast<ScreenValue>(2) * (value.x + static_cast<ScreenValue>(1)) - static_cast<ScreenValue>(0.5) ),
		static_cast<RasterValue>( static_cast<ScreenValue>(height) / static_cast<ScreenValue>(2) * (static_cast<ScreenValue>(1) - value.x) - static_cast<ScreenValue>(0.5) )
	};
}

Elite::WorldVector Elite::GetNormal(ObjectContainer::PtrVariant const& object, WorldPoint const& hitPoint, Intersection const& intersectionResult, WorldVector const& view)
{
	return std::visit(
		JL::Visitor{
			[](WorldObject<Plane> const* plane)
			{
				return plane->normal;
			},
			[&hitPoint](WorldObject<Sphere> const* sphere)
			{
				return hitPoint - sphere->center;
			},
			[&intersectionResult, &view](WorldObject<Mesh> const*)
			{
				auto const& triangle = *static_cast<Mesh::MeshData::Triangle const*>(intersectionResult.hitFace);
				auto const normal = Plane{ *triangle.x, *triangle.y, *triangle.z }.normal;
				if (Dot(normal, view) > 0)
					return -normal;
				else
					return normal;
			},
		},
		object
	);
}

Elite::SurfaceData const& Elite::GetSurfaceData(ObjectContainer::PtrVariant const& object)
{
	return std::visit(
		[](auto const* object) -> SurfaceData const&
		{
			return *object;
		},
		object
	);
}