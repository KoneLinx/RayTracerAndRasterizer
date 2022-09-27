#pragma once

#include <cstdint>
#include <SDL.h>
#include "JL/JL.h"
#include "DX11.h"

namespace Elite
{

	using PixelValue = uint32_t;
	using PixelSubValue = uint8_t;
	using ColourValue  = float;

	using RasterValue = size_t;
	using NDCValue = float;
	using UVValue = float;
	using WorldValue = float;

	constexpr unsigned DIMENTIONS = 3;
	constexpr unsigned COLOURS = 3;

	using RasterPoint  = Elite::Point  <2, RasterValue>;
	using NDCPoint     = Elite::Point  <2, NDCValue   >;
	using UVPoint      = Elite::Vector <2, UVValue    >;

	using WorldVector  = Elite::Vector <DIMENTIONS    , WorldValue>;
	using WorldPoint   = Elite::Vector <DIMENTIONS    , WorldValue>;
	using Projection   = Elite::Vector <DIMENTIONS + 1, WorldValue>;

	using Colour       = Elite::Vector <COLOURS, ColourValue >;


	NDCPoint    RasterToNCD    (const RasterPoint value, const RasterValue width, const RasterValue height);
	RasterPoint NDCToRaster    (const NDCPoint    value, const RasterValue width, const RasterValue height);
	Projection& PointToRaster  (      Projection& value, const RasterValue width, const RasterValue height);


	struct VertexData
	{
		UVPoint uv;
		WorldVector normal;
		WorldVector tangent;
	};

	using SurfaceManager = JL::SurfaceManager;
	using SurfaceID = SurfaceManager::SurfaceID;

	struct SurfaceData
	{
		SurfaceManager::SurfaceID diffuse;
		SurfaceManager::SurfaceID specular;
		SurfaceManager::SurfaceID gloss;
		SurfaceManager::SurfaceID normal;
	};

	struct FaceData
	{
		WorldPoint  const* point;
		UVPoint     const* uv;
		WorldVector const* normal;
		WorldVector const* tangent;
	};

	using Vertex       = JL::Aggregate <WorldPoint , VertexData >;
	using Face         = JL::Point     <3, JL::Aggregate<JL::Wrapper<Projection const*>, FaceData>>;

	using Camera       = JL::Camera    <DIMENTIONS , WorldValue  >;
													 		    
	using Triangle     = JL::Point     <3          , Vertex      >;
	using List         = Mesh_DX11;  //<Vertex     , unsigned long, WorldPoint >;
	using Strip        = JL::Strip     <             Vertex      >;
	//using Mesh         = JL::Mesh2     <WorldPoint , UVPoint      , WorldVector>;


	template<typename Type>
	using Object = JL::Aggregate<Type, SurfaceData>;

	using ObjectContainer = JL::VectorTuple<
		Object<Triangle>,
		Object<List>,
		Object<Strip>
		//Object<Mesh>
	>;
	

	PixelValue* GetPixels(SDL_Surface* pSurface);

	using RasterBoundingBox = std::pair<RasterPoint, RasterPoint>;


	struct CullMode
	{
		constexpr static bool single = true   ;
		constexpr static bool both   = !single;

		constexpr static bool front = false   ;
		constexpr static bool back  = true    ;
	};

	struct Scene
	{

		ObjectContainer objects;
		SurfaceManager surfaceMap;

	};


	struct RenderOptions
	{

		struct Rendermodes
		{
			using Type = bool;
			constexpr static bool SOFTWARE = false;
			constexpr static bool DIRECTX  = true;
		};
		Rendermodes::Type mode = Rendermodes::DIRECTX;

		SHADER::Culling culling  = SHADER::Culling::FRONT;
		SHADER::Sampler sampling = SHADER::Sampler::POINT;

		bool transparency   = true;

		bool renderNormal   = true;
		bool renderSpecular = true;

		bool renderDepth    = false;

	};

};