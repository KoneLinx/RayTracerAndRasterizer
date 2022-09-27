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

		void Render(const Camera& camera, Scene & scene, RenderOptions const& options);

		ID3D11Device* GetDevice();
		bool SaveBackbufferToImage() const;

	private:

		void RenderSoftware(const Camera& camera, Scene & scene, RenderOptions const& options);
		void RenderDirectX (const Camera& camera, Scene & scene, RenderOptions const& options);

		std::pair<HRESULT, StrRaw> InitializeDirectX();

		SDL_Window* m_pWindow = nullptr;
		RasterValue m_Width = 0;
		RasterValue m_Height = 0;

		bool m_IsInitialized;


		// Software

		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		std::vector<WorldValue> m_PixelDepthVector{};


		// DirectX

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;


		// Temporary

		Mesh_DX11* pTestmesh;

	};
}

#endif