#pragma once

#include <cstdint>
#include "Emath.h"
#include "JL/JL.h"

namespace Elite
{

	using PixelValue    = uint32_t;
	using PixelSubValue = uint8_t ;
	using ColourValue   = float   ;
	using RasterValue   = size_t  ;
	using ScreenValue   = float   ;
	using NDCValue      = float   ;
	using WorldValue    = float   ;

	constexpr unsigned DIMENTIONS = 3;
	constexpr unsigned COLOURS = 3;

	using RasterPoint  = Elite::Point  <2         , RasterValue>;
	using ScreenPoint  = Elite::Point  <2         , ScreenValue>;
	using NDCPoint     = Elite::Point  <2         , NDCValue   >;
	using WorldVector  = Elite::Vector <DIMENTIONS, WorldValue >;
	using WorldPoint   = Elite::Point  <DIMENTIONS, WorldValue >;
	using Colour       = Elite::Vector <COLOURS   , ColourValue>;

	struct SurfaceData
	{
		Colour colour = { 1, 1, 1 };
		ColourValue reflectance = 1;
		ColourValue specular = 1;
		ColourValue roughness = 1;
		bool nonmetal;
	};

	using JL::CullMode;

	struct RenderData
	{
		CullMode::Flag cullmode = CullMode::front;
	};
	
	using Ray              = JL::Ray             <DIMENTIONS, WorldValue>;
	using Intersection     = JL::Intersection    <DIMENTIONS, WorldValue>;
	using Sphere           = JL::Circular        <DIMENTIONS, WorldValue>;
	using Plane            = JL::Plane           <DIMENTIONS, WorldValue>;
	using PointLight       = JL::PointLight      <DIMENTIONS, WorldValue>;
	using DirectionalLight = JL::DirectionalLight<DIMENTIONS, WorldValue>;
	using Camera           = JL::Camera          <DIMENTIONS, WorldValue>;
	using Mesh             = JL::Mesh            <DIMENTIONS, WorldValue>;
	using Triangle         = Mesh::MeshData::Triangle;

	template<typename Object>
	//using WorldObject = Coloured<Object, COLOURS, ColourValue, Vector>;
	using WorldObject = JL::Agregate<Object, SurfaceData, RenderData>;

	using ObjectContainer      = JL::VectorTuple<
		WorldObject<Plane >,
		WorldObject<Sphere>,
		WorldObject<Mesh  >
	>;
	using LightsourceContainer = JL::VectorTuple<
		WorldObject<PointLight      >,
		WorldObject<DirectionalLight>
	>;

	struct Scene
	{
		ObjectContainer objects;
		LightsourceContainer lights;
	};

	struct RenderSettings
	{
		bool PBR;
		bool hardShadows;
		bool maxToAll;
	};

	NDCPoint   & RasterToNCD    (NDCPoint   & result, const RasterPoint value, const RasterValue width, const RasterValue height);
	RasterPoint& NDCToRaster    (RasterPoint& result, const NDCPoint    value, const RasterValue width, const RasterValue height);

	ScreenPoint& RasterToScreen (ScreenPoint& result, const RasterPoint value, const RasterValue width, const RasterValue height);
	RasterPoint& ScreenToRaster (RasterPoint& result, const ScreenPoint value, const RasterValue width, const RasterValue height);
	ScreenPoint  RasterToScreen                      (const RasterPoint value, const RasterValue width, const RasterValue height);
	RasterPoint  ScreenToRaster                      (const ScreenPoint value, const RasterValue width, const RasterValue height);


	WorldVector GetNormal(ObjectContainer::PtrVariant const& object, WorldPoint const& hitPoint, Intersection const& intersectionResult, WorldVector const& view);
	

	SurfaceData const& GetSurfaceData(ObjectContainer::PtrVariant const& object);

}