/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include "RenderUtils.h"
#include <vector>

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{

	class Renderer final
	{

		Renderer() = delete;

	public:

		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(const Camera& camera, Scene const& scene, RenderSettings const& settings);
		bool SaveBackbufferToImage() const;

	private:

		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		PixelValue* m_pBackBufferPixels = nullptr;
		std::vector<Colour> m_PixelColourVector{};
		RasterValue m_Width = 0;
		RasterValue m_Height = 0;

	};
}

#endif