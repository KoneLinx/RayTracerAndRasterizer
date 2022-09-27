// Lighting.h - Lighting functions.

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
#include "JLDirectionalLight.h"
#include "JLPointLight.h"

namespace JL
{

	template<int N, typename T>
	T LightIntensity(PointLight<N, T> const& source, T const& squareDistance)
	{
		return source.intensity / squareDistance;
	}

	template<int N, typename T>
	T LightIntensity(DirectionalLight<N, T> const& source, T const& /*discarded*/ = {})
	{
		return source.intensity;
	}


	namespace LMBR
	{


		template<int N, typename T>
		T LambertCosine(Vector<N, T> const& light, Vector<N, T> const& normal)
		{
			//  given: |n| = 1 and |l| = 1
			//  (l*n) = cos(l^n)
			return /*std::clamp(*/-Dot(light, normal) /*/ -sqrt(SqrMagnitude(light) * SqrMagnitude(normal))*//*, 0.f, 1.f)*/;
		}



		template<bool incomming = false, int N, typename T>
		T TestLMBR(Vector<N, T> const& ray, Vector<N, T> const& light, Vector<N, T> const& normal)
		{
			return 1;
			const Vector<N, T> reflection{ Elite::Reflect(light, normal) };
			return std::clamp(-Dot(reflection, ray), 0.f, 1.f);
		}



		template<int N, typename T>
		T Phong(Vector<N, T> const& view, Vector<N, T> const& light, T const& phongExp)
		{
			// Pe: Phong exp. // Ks: Specular reflectance factor
			//  (r*n) ^ Pe * Ks = Phong Intensity
			return std::pow(Dot(view, light), phongExp);
		}

	}

	namespace PBR
	{

		template<typename T>
		constexpr T FRESNEL_DEFAULT = static_cast<T>(0.4);
		
		template<typename T>
		T TrowbridgeReitzGGX(T const& normalHalfDot, T const& squareRoughness)
		{
			// Normal distribution
			// a: Roughness^2 // n: Normal // h: Half vector
			//   a^2 / ( pi * ( (n*h)^2 * (a^2 -1) + 1 )^2 )
			const T roughness = squareRoughness * squareRoughness;
			return (roughness / 3.14159265358979323846264338327950288) / Square(normalHalfDot * normalHalfDot * (roughness - 1) + 1); // M_PI was acting weird
		}

		template<int N, typename T>
		T TrowbridgeReitzGGX(Vector<N, T> const& normal, Vector<N, T> const& half, T const& squareRoughness)
		{
			return TrowbridgeReitzGGX(Dot(normal, half), squareRoughness);
		}



		template<typename Colour, typename WorldValue>
		Colour Schlick(WorldValue const& normalHalfDot, Colour const& fresnelBase)
		{
			// fresnel function
			// n: Normal // h: Half vector // F: Fresnel base 
			//  F + (1 - F) * ( 1 - (n*h) )^5
			return fresnelBase + (1 - fresnelBase) * pow(1 - normalHalfDot, 5);
		}

		template<typename Colour, int DIMENTIONS, typename WorldValue>
		Colour Schlick(Vector<DIMENTIONS, WorldValue> const& normal, Vector<DIMENTIONS, WorldValue> const& half, Colour const& fresnelBase)
		{
			return Schlick(Dot(normal, half), fresnelBase);
		}



		template<bool DIRECT = true, typename T>
		T RoughnessRemap(T const& roughnessSquared)
		{
			return Conditional<DIRECT>(
				Square(roughnessSquared + 1) / 8,
				Square(roughnessSquared) / 2
			);
		}



		template<typename T>
		T SchlickGGX(T const& normalViewDot, T const& roughnessRemap)
		{
			// Geometry function
			// n: Normal // h: Half vector // k: Roughness remap
			//   (n*h) / ( (n*h) * (1 - k) + k )
			return normalViewDot / (normalViewDot * (1 - roughnessRemap) + roughnessRemap);
		}

		template<int N, typename T>
		T SchlickGGX(Vector<N, T> const& normal, Vector<N, T> const& half, T const& roughnessRemap)
		{
			return SchlickGGX<T>(Dot(normal, half), roughnessRemap);
		}



		template<typename T>
		T Smith(T const& normalViewDot, T const& normalLightDot, T const& roughnessRemap, T(*const geometryFunction)(T const&,T const&))
		{
			return geometryFunction(normalViewDot, roughnessRemap) * geometryFunction(normalLightDot, roughnessRemap);
		}



		template<bool DIRECT = true, bool PLASTIC = false, int COLOURS = 3, typename ColourValue = void, int DIMENTIONS, typename WorldValue>
		auto CookTorrance(Vector<DIMENTIONS, WorldValue> const& normal, Vector<DIMENTIONS, WorldValue> const& light, Vector<DIMENTIONS, WorldValue> const& view, WorldValue const& roughnessSquared, Vector<COLOURS, ColourValue> const& fresnelBase = {})
		{
			// BRDF Cook-Torrance
			//
			//       D * F * G / 4 / (v*n) / (l*n)
			//  <=>  D * G / 4 / (n*v) / (l*n) * F

			constexpr bool DEFAULT_FERSNEL = std::is_same<ColourValue, void>::value;
			constexpr bool NONMETAL = PLASTIC | DEFAULT_FERSNEL;
			//defaults when either PLASTIC is true or fersnelBase is defaulted.

			using ColourValue_t = std::conditional_t<DEFAULT_FERSNEL, WorldValue, ColourValue>;
			using Colour = std::conditional_t<NONMETAL, ColourValue_t, Vector<COLOURS, ColourValue_t>>;

			constexpr auto D = TrowbridgeReitzGGX<WorldValue>;
			constexpr auto F = Schlick<Colour, WorldValue>;
			constexpr auto G = SchlickGGX<WorldValue>;
			constexpr auto GSmith = Smith<WorldValue>;

			auto const halfVector = GetNormalized(HalfVector(light, view));
			WorldValue const normalLightDot = -Dot(normal, light);
			WorldValue const normalViewDot = -Dot(normal, view);
			WorldValue const normalHalfDot = -Dot(normal, halfVector);
			WorldValue const halfViewDot = Dot(halfVector, view);
			WorldValue const roughnessRemap = RoughnessRemap(roughnessSquared);

			std::conditional_t<NONMETAL, Colour const, Colour const&> fresnel =
				Conditional<NONMETAL>(
					FRESNEL_DEFAULT<ColourValue_t>,
					fresnelBase
				);
 
			const Colour fresnelResult{ F(halfViewDot, fresnel) };
			const ColourValue_t factorResult{
					D(normalHalfDot, roughnessSquared) * GSmith(normalViewDot, normalLightDot, roughnessRemap, G) // D * G
					/ static_cast<WorldValue>(4) / normalLightDot / normalViewDot // 4 * nv * nl
			};
			const Colour result = factorResult * fresnelResult;

			return Conditional<NONMETAL>(
				std::pair<Colour, Colour>{ result, fresnelResult },
				result
			);

		}


	};



	template<int N, typename T>
	Vector<N, T> Shade_Lambert_CookTorrance(Vector<N, T> const& normal, Vector<N, T> const& light, Vector<N, T> const& view, T const& specularFactor, T const& roughnessSquared, bool isNonMetal = true, Vector<N, T> const& fresnelBase = {})
	{

		constexpr auto diffuseFunction = LMBR::LambertCosine<N, T>;
		
		if (isNonMetal)
		{

			constexpr auto specularFunction = PBR::CookTorrance<true, true, N, T, N, T>;
		
			const T diffuse = diffuseFunction(light, normal);
			const auto [specular, fresnel] = specularFunction(normal, light, view, roughnessSquared, fresnelBase);
			const T result{ (1 - fresnel) * diffuse + specularFactor * specular };
			return { result, result, result };

		}
		else
		{
		
			constexpr auto specularFunction = PBR::CookTorrance<true, false, N, T, N, T>;
			
			const auto specular = specularFunction(normal, light, view, roughnessSquared, fresnelBase);
			return specular * specularFactor;

		}

	}
}