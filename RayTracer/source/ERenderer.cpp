//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include <memory>
using namespace Elite;

//globals... I know

//ObjectContainer g_WorldObjects{
//
//	WorldObject<Plane> { { { 0, -2, 0 }, { 0,  1, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
//	WorldObject<Plane> { { { 0, 10, 0 }, { 0, -1, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
//	WorldObject<Plane> { { { -5, 0, 0 }, {  1, 0, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
//	WorldObject<Plane> { { {  5, 0, 0 }, { -1, 0, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
//	WorldObject<Plane> { { { 0, 0,  5 }, { 0, 0, -1 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
//	WorldObject<Plane> { { { 0, 0, -5 }, { 0, 0,  1 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
//
//	WorldObject<Sphere>{ { { -2, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, .1f, true }, {} },
//	WorldObject<Sphere>{ { {  0, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, .6f, true }, {} },
//	WorldObject<Sphere>{ { {  2, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, 1.f, true }, {} },
//
//	WorldObject<Sphere>{ { { -2, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, .1f, false }, {} },
//	WorldObject<Sphere>{ { {  0, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, .6f, false }, {} },
//	WorldObject<Sphere>{ { {  2, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, 1.f, false }, {} },
//
//};
//
//LightsourceContainer g_WorldLights{
//
//	WorldObject<PointLight>{ { { -1, 5, 3.f  }, 10 }, { { 1.f, 1.f, .8f } }, {} },
//	WorldObject<PointLight>{ { { 1, 2, -2.5f }, 10 }, { { .8f, .8f, 1.f } }, {} },
//	WorldObject<PointLight>{ { { 1, 6, -1.f  }, 10 }, { { .8f, .8f, 1.f } }, {} },
//	WorldObject<DirectionalLight>{ { { -.5f, -1.f, -.5f }, 1.f }, { { 1.f, 1.f, 1.f } }, {} }
//
//};

Elite::Renderer::Renderer(SDL_Window * pWindow)
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<RasterValue>(width);
	m_Height = static_cast<RasterValue>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, int(m_Width), int(m_Height), 32, 0, 0, 0, 0);
	m_pBackBufferPixels = static_cast<PixelValue*>(m_pBackBuffer->pixels);

	m_PixelColourVector.resize( m_Width * m_Height );

	//Mesh mesh{};
	//JL::LoadMesh(mesh, R"(triangle.obj)");

	//mesh.Translate({ -2, 4, 0 });
	//g_WorldObjects += WorldObject<Mesh>{ mesh, { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::front } };
	//
	//mesh.Translate({ 2, 0, 0 });
	//g_WorldObjects += WorldObject<Mesh>{ mesh, { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::both } };
	//
	//mesh.Translate({ 2, 0, 0 });
	//g_WorldObjects += WorldObject<Mesh>{ std::move(mesh), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::back } };

}

template<typename T, typename ...R>
T BP(T && c, R const& ...)
{
	return c;
}

void Elite::Renderer::Render(const Camera& camera, Scene const& scene, RenderSettings const& settings)
{

	// some render constants
	//constexpr bool MAX_TO_ONE = true;
	ColourValue high{ 0 }; // when max to all, track max value

	// Setup values

	SDL_LockSurface(m_pBackBuffer);

	ScreenPoint screenPoint{}; // tmp value
	Ray ray{ camera.GetRayOrigin() }; // main cast ray

	// origin point
	RasterToScreen(screenPoint , RasterPoint{ 0,0 }, m_Width, m_Height);
	
	// x increment for main loop
	WorldVector xIncrement{
		RasterToScreen(RasterPoint{ 1,0 }, m_Width, m_Height) - screenPoint
	};
	camera.ViewToWorld(xIncrement);
	
	// y increment for main loop
	WorldVector yIncrement{
		RasterToScreen(RasterPoint{ 0,1 }, m_Width, m_Height) - screenPoint
	};
	camera.ViewToWorld(yIncrement);

	// (x * width + y) increment for return in main loop
	WorldVector yRetun{ xIncrement * -static_cast<WorldValue>(m_Width) + yIncrement };
	
	// setup origin ray
	ray.direction = {
		screenPoint.x,
		screenPoint.y,
		static_cast<WorldValue>(1)
	};
	camera.ViewToWorld(ray.direction);
	
	// default colour = black
	const Colour defaultColour{};
	
	//
	// MAIN LOOP: Casting ray for each pixel
	//
	for (RasterPoint point{}; point.y < m_Height; ray.direction += yRetun, ++point.y)
	{
		for (point.x = 0; point.x < m_Width; ray.direction += xIncrement, ++point.x)
		{
			// per pixel variables

			ObjectContainer::PtrVariant hitObject{};
			Colour lightColour{};
			Intersection t{ Ray::tMax };

			// First hit loop function
			
			JL::Visitor const hitFunction{
				[&t, &ray, &hitObject](auto const& object)
				{
					if (object.cullmode == JL::CullMode::none)
						return;
					Intersection intersection;
					if (Intersect(intersection, ray, object, object.cullmode) && intersection < t)
					{
						t = intersection;
						hitObject = &object;
					}
				}
			};

			scene.objects.ForEach(hitFunction);
			
			// When hit
			if (t < Ray::tMax)
			{

				// Register hit information

				const WorldPoint hitPoint{ ray(t) };
				struct HitInfo {
					WorldPoint const& position;
					WorldVector const surfaceNormal;
					WorldVector const& incommingRayDirection;
					SurfaceData const& surface;
				} const hitInfo{
					hitPoint,
					GetNormalized(GetNormal(hitObject, hitPoint, t, ray.direction)),
					GetNormalized(ray.direction),
					GetSurfaceData(hitObject)
				};
				


				// Direct lighting calculation function

				JL::Visitor const shadeCalc{
					[&lightColour, &hitInfo, &settings](auto const& lightSource, WorldVector const& distance)
					{
						const WorldValue squareDistance{ SqrMagnitude(distance) };
						const WorldVector light = distance / sqrt(squareDistance);
						
						if (!settings.PBR)
						// LMBR
						{
							const WorldVector reflection = Elite::Reflect(light, hitInfo.surfaceNormal);

							WorldValue intensity{};
							intensity += hitInfo.surface.roughness * JL::LMBR::LambertCosine(light, hitInfo.surfaceNormal);
							intensity += (1 - hitInfo.surface.roughness) * JL::LMBR::Phong(hitInfo.incommingRayDirection, reflection, 60.f);

							intensity *= 0.35f; // LMBR is more sensitive to intense light. This allows for a more convincing look without changing the scene's light's values.

							lightColour += lightSource.colour * (JL::LightIntensity(lightSource, squareDistance) * intensity);
						}
						else
						// PBR
						{
							lightColour +=
								JL::GetScaled(
									lightSource.colour * (JL::LightIntensity(lightSource, squareDistance) * JL::LMBR::LambertCosine(light, hitInfo.surfaceNormal)),
									JL::Shade_Lambert_CookTorrance(hitInfo.surfaceNormal, light, hitInfo.incommingRayDirection, hitInfo.surface.specular, Square(hitInfo.surface.roughness), hitInfo.surface.nonmetal, hitInfo.surface.colour)
								);
						}
					}
				};

				// Light hit function

				JL::Visitor const lightHitFunction{
					// Point light
					[&scene, &shadeCalc, &hitInfo, &ray] (WorldObject<PointLight> const& source)
					{
						const Ray lightRay{ source.position, hitInfo.position };
						if (Dot(hitInfo.surfaceNormal, lightRay.direction) < 0)
						{
							const bool hit{ scene.objects.AnyOf(
								[&lightRay](auto const& object) -> bool
								{
									if (object.cullmode == JL::CullMode::none)
										return false;
									Intersection t{};
									return JL::Intersect<false>(t, lightRay, object, JL::CullMode::both) && t < 1.f - Ray::tMin;
								}
							) };
							if (!hit)
								shadeCalc(source, lightRay.direction);
						}
					},
					// Directional light
					[&scene, &shadeCalc, &hitInfo](WorldObject<DirectionalLight> const& source)
					{
						const Ray lightRay{ hitInfo.position, source.direction }; // We cast away from the light, but test for hits behind
						if (Dot(hitInfo.surfaceNormal, lightRay.direction) > 0)
						{
							const bool hit{ scene.objects.AnyOf(
								[&lightRay](auto const& object) -> bool
								{
									if (object.cullmode == JL::CullMode::none)
										return false;
									return JL::Intersect<true>(lightRay, object, JL::CullMode::both);
								}
							) };
							if (!hit)
								shadeCalc(source, source.direction);
						}
					}
				};

				// Light No Shadow function

				JL::Visitor const lightNoHitFunction{
					// Point light
					[&shadeCalc, &hitInfo](WorldObject<PointLight> const& source)
					{
						WorldVector const light{ hitInfo.position - source.position  };
						if (Dot(hitInfo.surfaceNormal, light) < 0)
							shadeCalc(source, light);
					},
					// Directional light
					[&shadeCalc, &hitInfo](WorldObject<DirectionalLight> const& source)
					{
						WorldVector const& light{ source.direction  };
						if (Dot(hitInfo.surfaceNormal, light) < 0)
							shadeCalc(source, light);
					},
				};

				if (settings.hardShadows)
					scene.lights.ForEach(lightHitFunction);
				else
					scene.lights.ForEach(lightNoHitFunction);
				
				// Setting final pixel colour

				if (hitInfo.surface.nonmetal)
					JL::Scale(lightColour, hitInfo.surface.colour * hitInfo.surface.reflectance);
				
				ColourValue const max = std::max(lightColour.r, std::max(lightColour.g, lightColour.b));
				
				if (!settings.maxToAll) // constexpr (MAX_TO_ONE)
				{
					if (max > 1)
						lightColour /= max;
				}
				else
				{
					if (max > high)
						high = max;
				}

				m_PixelColourVector[point.x + (point.y * m_Width)] = lightColour;
			}
			else
				m_PixelColourVector[point.x + (point.y * m_Width)] = defaultColour;

		}
	}

	// Normalize all colour values

	ColourValue factor{ 255.f / (settings.maxToAll ? high : 1) }; //JL::Conditional<MAX_TO_ONE>(1, high) };
	
	std::transform(
		begin(m_PixelColourVector), end(m_PixelColourVector), m_pBackBufferPixels,
		[factor, format = m_pBackBuffer->format] (Colour const& colour)
		{
			return SDL_MapRGB(
				format,
				static_cast<PixelSubValue>(colour.r * factor),
				static_cast<PixelSubValue>(colour.g * factor),
				static_cast<PixelSubValue>(colour.b * factor)
			);
		}
	);


	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}