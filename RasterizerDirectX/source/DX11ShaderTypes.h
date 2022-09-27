#pragma once

#include "JL/JLUtility.h"

namespace shader
{

	// Index

	using Index = unsigned long;  // 32 bit integral;

	constexpr DXGI_FORMAT FLOAT_FORMAT[]
	{
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
	};

	constexpr DXGI_FORMAT UINT_FORMAT[]
	{
		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32A32_UINT,
	};



	// float

	template <size_t N>
	struct Float : Elite::Vector<N, float>
	{
		using Elite::Vector<N, float>::Vector;
		constexpr static auto FORMAT = FLOAT_FORMAT[N - 1];
		constexpr static auto SIZE   = sizeof(float[N]);
	};

	template <>
	struct Float<1> : JL::Wrapper<float>
	{
		using JL::Wrapper<float>::Wrapper;
		constexpr static auto FORMAT = FLOAT_FORMAT[0];
		constexpr static auto SIZE   = sizeof(float[1]);
	};

	using float1 = Float<1>;
	using float2 = Float<2>;
	using float3 = Float<3>;
	using float4 = Float<4>;

	using float3x3 = Matrix<3, 3, float>;
	using float4x4 = Matrix<4, 4, float>;



	// Index

	using Index = unsigned long; // 32-bit intergal

	struct Uint : JL::Wrapper<Index>
	{
		using JL::Wrapper<Index>::Wrapper;
		constexpr static auto FORMAT = UINT_FORMAT[0];
		constexpr static auto SIZE   = sizeof(Index);
	};
	using uint = Uint;



	// Construct layout

	template <typename ... Float, typename ... Name>
	constexpr std::array<D3D11_INPUT_ELEMENT_DESC, sizeof...(Float)> VertexDescLayout(Name ... name) noexcept
	{
		auto element = [alignment = size_t()](auto const format, auto const size, auto name) mutable
		{
			size_t const align = alignment;
			alignment += size;
			D3D11_INPUT_ELEMENT_DESC layout{};
			layout.SemanticName = name;
			layout.Format = format;
			layout.AlignedByteOffset = align;
			layout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			return layout;
		};
		return { element(Float::FORMAT, Float::SIZE, name) ... };
	}

}