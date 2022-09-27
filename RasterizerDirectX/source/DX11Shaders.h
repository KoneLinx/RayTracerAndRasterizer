#pragma once
#include "DX11ShaderTypes.h"

namespace shader::ext::shaders
{
	using shader::Index;

	namespace poscol
	{
		using shader::Index;

		constexpr WStrLiteral FILENAME = L"PosCol3D.fx";

		struct Vertex
		{
			float3 position;
			float3 color;
		};

		constexpr StrLiteral TECHNIQUE_DEFAULT = "DefaultTechnique";
		constexpr StrLiteral VAR_VIEWMATRIX    = "gViewTransformation";

		constexpr auto VERTEX_DESC = VertexDescLayout
		<	float3    ,	float3	>
		(	"POSITION",	"COLOR"	);

	}

	namespace rasterizer
	{
		using shader::Index;

		constexpr WStrLiteral FILENAME = L"Rasterizer.fx";

		struct Vertex
		{
			float3 pos;
			float2 uv;
			float3 normal;
			float3 tangent;
		};

		constexpr auto VERTEX_DESC = VertexDescLayout
		<	float3		, float2	, float3	, float3	>
		(   "POSITION"	, "UV"		, "NORMAL"	, "TANGENT"	);

		constexpr StrLiteral TECHNIQUE_DEFAULT = "DefaultTechnique";
											
		constexpr StrLiteral VAR_VIEW_MATRIX   = "VIEW_TRANSFORMATION";
		constexpr StrLiteral VAR_VIEW_ORIGIN   = "VIEW_ORIGIN";
		constexpr StrLiteral VAR_VIEW_ROTATION = "VIEW_ROTATION";

		constexpr StrLiteral VAR_DIIFUSE_MAP   = "DIFFUSE_MAP";
		constexpr StrLiteral VAR_SPECULAR_MAP  = "SPECULAR_MAP";
		constexpr StrLiteral VAR_NORMAL_MAP    = "NORMAL_MAP";

		enum struct Sampler
		{
			POINT,
			LINEAR,
			ANISOTROPIC
		};

		enum struct Culling
		{
			FRONT,
			BACK,
			BOTH
		};

	}

};

using namespace shader::ext;
