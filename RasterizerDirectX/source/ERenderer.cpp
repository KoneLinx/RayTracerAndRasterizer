#include "pch.h"

//Project includes
#include "ERenderer.h"
#include "JL/Devel.h"
#include "Elite/ERGBColor.h"
using namespace Elite;

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
	m_PixelDepthVector.resize( m_Width * m_Height );

	auto const [result, message] = InitializeDirectX();
	if (FAILED(result))
	{
		puts("[error] DirectX failed to initialize\nThrowing...");
		puts(message);
		throw std::exception{ "Failed to initialize DirectX" };
	}
	else
	{
		//puts(message);
		m_IsInitialized = true;
	}

}


void Elite::Renderer::RenderSoftware(const Camera& camera, Scene & scene, RenderOptions const& options)
{

	// Start reading at the bottom. The top is a collection of lambda declarations.

	JL::Visitor const mapColour{
		[pixels = GetPixels(m_pBackBuffer), format = m_pBackBuffer->format] (size_t const index, Colour const& colour)
		{
			constexpr ColourValue factor{ 0xFF };
			return pixels[index] = SDL_MapRGB(
				format,
				static_cast<PixelSubValue>(colour.r * factor),
				static_cast<PixelSubValue>(colour.g * factor),
				static_cast<PixelSubValue>(colour.b * factor)
			);
		},
		[pixels = GetPixels(m_pBackBuffer), format = m_pBackBuffer->format](size_t const index, PixelValue const colour)
		{
			return pixels[index] = colour;
		}
	};
	
	auto constexpr pixelIndex =
		[](UVPoint const& uv, auto w, auto h)
		{
			return int(uv.x * w) % w + int(uv.y * h) % h * w;
		};

	auto const& surfaceMap = scene.surfaceMap;
	auto const samplePixel =
		[format = m_pBackBuffer->format, &surfaceMap, &pixelIndex](SurfaceID id, UVPoint const& uv) -> Colour
		{
			if (!id) return {};

			auto const pSurface = surfaceMap[id].pSDLSurface;
			
			PixelValue const pixel = GetPixels(pSurface)[pixelIndex(uv, pSurface->w, pSurface->h)];
			PixelSubValue r, g, b;
			SDL_GetRGB(pixel, pSurface->format, &r, &g, &b);

			constexpr auto toFloat =
				[](PixelSubValue value)
				{
					return float(value) / float(0xFF);
				};

			return {
				toFloat(r),
				toFloat(g),
				toFloat(b)
			};
		};

	auto const index =
		[w = m_Width, s = m_Width * m_Height] (RasterPoint const& p) -> size_t
		{
			return (p.x + p.y * w) % s;
		};

	static WorldVector lightDirection{ .577f, -.577f, -.577f };
	//lightDirection *= MakeRotationY(0.01f);
	WorldVector const& lightDir{ lightDirection };

	auto const fillPixel =
		[this, &index, &mapColour, &samplePixel, &camera, lightDir, &options](RasterPoint const& point, WorldValue const& depth, VertexData const& vertex, WorldVector const view, auto const& object)
		{
			size_t i = index(point);
			if (depth < m_PixelDepthVector[i])
			{
				m_PixelDepthVector[i] = depth;
				if (!options.renderDepth)
				{
					Colour colour  = samplePixel(object.diffuse , vertex.uv);
					
					Colour specular = samplePixel(options.renderSpecular ? object.specular : 0, vertex.uv);
					
					bool const renderNormal{ options.renderNormal && object.normal };
					Colour normal =
						renderNormal
						? samplePixel(object.normal, vertex.uv)
						: vertex.normal;
					if (renderNormal)
					{
						normal -= Colour{ .5, .5, .5 };
						normal *= 2;
						Normalize(normal);
					}
					auto const newNormal = renderNormal
						?	Matrix<3, 3, WorldValue>{
								Cross(vertex.tangent, vertex.normal),
								vertex.tangent,
								vertex.normal
							} * normal
						:	normal;

					Colour const lightColour{ 1.f, 1.f, 1.f };
					float const lightIntesity{ 3.f };

					float const lambert = std::max(0.f, -Dot(newNormal, lightDir));
					auto const light = lightColour * (lambert * lightIntesity);
					auto const diffuse = lightColour * (0.75f * lambert);

					if (options.renderSpecular && object.specular)
					{
						WorldVector const reflection = Reflect(lightDir, newNormal);
						specular *= Phong(view, reflection, 1.f);
					}

					mapColour(
						i,
						MaxToOne(
							JL::Scale(
								colour,
								light,
								specular + diffuse
							)
						)
					);

				}
				else
				{
					ColourValue const value{ 1.f / (.618033988f + Remap(depth, camera.VIEW_NEAR, camera.VIEW_FAR)) - .618033988f };
					//ColourValue const value{ std::min(1.f, 2.f / depth) };
					mapColour(i, Colour{ value, value, value });
				}
			}
		};

	auto constexpr forEachPixel =
		[] (auto const& function, RasterBoundingBox const& bounds) -> void
		{
			auto const& [tl, br] = bounds;
			for (RasterPoint point{ tl }; point.y <= br.y; ++point.y)
				for (point.x = tl.x; point.x <= br.x; ++point.x)
					function(point);
		};

	auto const renderTriangle =
		[this, &forEachPixel, &fillPixel, &camera, &options](Face const& face, RasterBoundingBox const& box, auto const& object)
		{
			WorldVector const defaultNormal{
				[&face] {
					if (face.x.normal)
						return WorldVector{};
					WorldVector normal{ GetNormalized(Cross(*face.y.point - *face.x.point, *face.z.point - *face.x.point)) };
					std::swap(normal.x, normal.z);
					return normal;
				}()
			};

			forEachPixel(
				[&](RasterPoint const& point)
				{
					bool render{ true };
					Vector<3, WorldValue> result;

					if (options.culling == SHADER::Culling::FRONT || options.culling == SHADER::Culling::BOTH)
					{
						auto [ hit , hitResult  ] = PointInTriangle<false>(point, face);
						hit &= hit;
						result = hitResult;
					}
					else if (options.culling == SHADER::Culling::BACK || options.culling == SHADER::Culling::BOTH)
					{
						auto [ hit , hitResult  ] = PointInTriangle<true>(point, face);
						hit &= hit;
						result = hitResult;
					}

					if (render)
					{
						Vector<3, WorldValue> const depths{ face.x->w, face.y->w, face.z->w };
						auto const depth{ DepthInterpolation(result, depths) };
						
						auto const uvInterpolated = 
							DepthInterpolation<UVPoint>(
								result,
								depths,
								{ *face.x.uv, *face.y.uv, *face.z.uv }
							) * depth;
						auto const normalInterpolated = face.x.normal ?
							GetNormalized(
								DepthInterpolation<WorldVector>(
									result,
									depths,
									{ *face.x.normal, *face.y.normal, *face.z.normal }
								)
							) : defaultNormal;
						auto const tangentInterpolated = options.renderNormal && object.normal && face.x.tangent ?
							GetNormalized(
								DepthInterpolation<WorldVector>(
									result,
									depths,
									{ *face.x.tangent, *face.y.tangent, *face.z.tangent }
								)
							) : WorldVector{};
						auto const pointInterpolated = 
							DepthInterpolation<WorldVector>(
								result,
								depths,
								{ WorldVector{*face.x.point}, WorldVector{*face.y.point}, WorldVector{*face.z.point} }
							) * depth;

						WorldVector const view{ GetNormalized(WorldPoint{ pointInterpolated } - camera.GetRayOrigin()) };
						
						fillPixel(
							point,
							depth,
							{
								uvInterpolated,
								normalInterpolated,
								tangentInterpolated
							},
							view,
							object
						);
					}
				},
				box
			);
		};

	auto const isInFrustum =
		[&camera, w = m_Width, h = m_Height](Face const& face) -> bool
	{
		return (
			JL::Min(face.x->x, face.y->x, face.z->x) > 0 && JL::Max(face.x->x, face.y->x, face.z->x) < w &&
			JL::Min(face.x->y, face.y->y, face.z->y) > 0 && JL::Max(face.x->y, face.y->y, face.z->y) < h &&
			JL::Min(face.x->w, face.y->w, face.z->w) > camera.VIEW_NEAR &&
			JL::Max(face.x->w, face.y->w, face.z->w) < camera.VIEW_FAR
			);
	};

	auto const isInFrustumDepth =
		[&camera](Face const& face) -> bool
	{
		return (
			JL::Min(face.x->w, face.y->w, face.z->w) > camera.VIEW_NEAR &&
			JL::Max(face.x->w, face.y->w, face.z->w) < camera.VIEW_FAR
		);
	};

	auto const isFullyOnRaster =
		[w = m_Width, h = m_Height](RasterBoundingBox const& box) -> bool
	{
		auto const& [tl, br] = box;
		return std::min(tl.x, tl.y) > 0 && br.x < w&& br.y < h;
	};

	auto const isPartOnRaster =
		[w = m_Width, h = m_Height](RasterBoundingBox const& box) -> bool
	{
		auto const& [tl, br] = box;

		auto const [xL, xH] = std::minmax(tl.x, br.x);
		auto const [yL, yH] = std::minmax(tl.y, br.y);
		if ((xH < 0 || xL >= w) || (xH < 0 || yL >= h))
			return false;

		return true;
	};

	auto const prepareTriangle =
		[&renderTriangle, &isInFrustumDepth, &isFullyOnRaster, w = m_Width, h = m_Height](Face const face, auto const& object)
		{
			if (!isInFrustumDepth(face))
				return;
			
			auto const box = GetRasterBoundingBox(face, w, h);
			if (isFullyOnRaster(box))
				return renderTriangle(face, box, object);

			return;
			
			// If partially on raster ...

		};

	JL::Visitor const rasterizationStage{

		// Triangle
		[&prepareTriangle](auto const& points, Object<Triangle> const& object)
		{
			prepareTriangle(
				Face{
					{ &points.x, { &object.x, &object.x.uv, nullptr, nullptr } },
					{ &points.y, { &object.y, &object.y.uv, nullptr, nullptr } },
					{ &points.z, { &object.z, &object.z.uv, nullptr, nullptr } }
				},
				object
			);
		},

		// List
		[&prepareTriangle](auto const& points, Object<List> const& object)
		{
			for (auto const& face : object.faces)
				prepareTriangle(
					Face{
						{ &points[face.x], { &object.vertices[face.x].pos, &object.vertices[face.x].uv, &object.vertices[face.x].normal, &object.vertices[face.x].tangent } },
						{ &points[face.y], { &object.vertices[face.y].pos, &object.vertices[face.y].uv, &object.vertices[face.y].normal, &object.vertices[face.y].tangent } },
						{ &points[face.z], { &object.vertices[face.z].pos, &object.vertices[face.z].uv, &object.vertices[face.z].normal, &object.vertices[face.z].tangent } }
					},
					object
				);
		},

		// Mesh
		//[&prepareTriangle](auto const& points, Object<Mesh> const& object)
		//{
		//	for (auto const& face : object.faces)
		//		prepareTriangle(
		//			Face{
		//				{ &points[face.x.v], { &object.vertices[face.x.v], &object.uvs[face.x.t], &object.normals[face.x.n], nullptr } },
		//				{ &points[face.y.v], { &object.vertices[face.y.v], &object.uvs[face.y.t], &object.normals[face.y.n], nullptr } },
		//				{ &points[face.z.v], { &object.vertices[face.z.v], &object.uvs[face.z.t], &object.normals[face.z.n], nullptr } },
		//			},
		//			object
		//		);
		//},

		// Strip
		[&prepareTriangle](auto const& points, Object<Strip> const& object)
		{

			auto const end{ object.indeces.data() + (object.indeces.size() - 2u) };
			bool odd{ false };

			union
			{
				Strip::Index const* pos;
				struct {
					Strip::Index const x, y, z;
				} const*const val;
			}
			face{};

			for (face.pos = object.indeces.data(); face.pos < end; ++face.pos, odd ^= true)
			if  (face.val->x != face.val->y && face.val->y != face.val->z)
			{
				auto const [y, z] = odd ? std::pair{ face.val->z, face.val->y } : std::pair{ face.val->y, face.val->z };
				prepareTriangle(
					Face{
						{ &points[face.val->x], { &object.vertices[face.val->x], &object.vertices[face.val->x].uv, nullptr, nullptr } },
						{ &points[y]          , { &object.vertices[y]          , &object.vertices[y]          .uv, nullptr, nullptr } },
						{ &points[z]          , { &object.vertices[z]          , &object.vertices[z]          .uv, nullptr, nullptr } },
					},
					object
				);
			}

		}

	};

	auto const projectPoints =
		[this, &rasterizationStage, &camera](auto& points, auto const& object) -> void
		{
				rasterizationStage(PointsToRaster(ProjectPoints(TransformToView(points, camera)), m_Width, m_Height), object);
		};

	JL::Visitor const projectionStage{

		// Triangle
		[&projectPoints] (Object<Triangle> const& object)
		{
			Triangle const& triangle{ object };
			Point<3, Projection> points{ triangle.x, triangle.y, triangle.z };
			projectPoints(points, object);
		},

		// List, Strip and Mesh
		[&projectPoints] (Object<List> const& object)
		{
			if (object.IsTransparant()) return;
			auto points{ CopyVertices<Projection>(object.vertices) };
			projectPoints(points, object);
		},

		// Other
		[&projectPoints] (auto const& object)
		{
			auto points{ CopyVertices<Projection>(object.vertices) };
			projectPoints(points, object);
		}

	};

	// Fill buffer with blackness
	std::fill_n(GetPixels(m_pBackBuffer), m_PixelDepthVector.size(), 0);
	// Fill depth buffer with flt_max
	std::fill(begin(m_PixelDepthVector), end(m_PixelDepthVector), std::numeric_limits<WorldValue>::max());

	// Main call
	scene.objects.ForEach(projectionStage);

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Elite::Renderer::RenderDirectX(const Camera& camera, Scene& scene, RenderOptions const& options)
{

	if (!m_IsInitialized)
		return;

	shader::float4x4 const* pViewTransformation = &camera.GetViewTransformation();
	shader::float3x3 const* pViewRotation       = &camera.GetViewRotation();
	shader::float3   const* pViewOrigin         = &reinterpret_cast<shader::float3 const&>(camera.GetRayOrigin());

	Vector<4, float> const defaultColor{ 54 / 255.f, 57 / 255.f, 63 / 255.f };

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, defaultColor.data);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	// RENDERING

	for (Object<List>& mesh : scene.objects.Get<Object<List>>())
		mesh.Render(
			m_pDeviceContext,
			options.culling,
			options.sampling,
			options.transparency,
			pViewTransformation,
			pViewRotation,
			pViewOrigin,
			scene.surfaceMap[mesh.diffuse ].pDXResourceView,
			scene.surfaceMap[mesh.specular].pDXResourceView,
			scene.surfaceMap[mesh.normal  ].pDXResourceView
		);

	m_pSwapChain->Present(0, 0);

}

void Elite::Renderer::Render(const Camera& camera, Scene& scene, RenderOptions const& options)
{
	switch (options.mode)
	{
	case RenderOptions::Rendermodes::SOFTWARE:
		return RenderSoftware(camera, scene, options);
	case RenderOptions::Rendermodes::DIRECTX:
		return RenderDirectX (camera, scene, options);
	}
}

ID3D11Device* Elite::Renderer::GetDevice()
{
	if (!m_IsInitialized)
	{
		puts("[error] Tried to access resources, but DirectX was not initialized.\nThrowing...");
		throw std::exception{ "DirectX was not initialized" };
	}
	return m_pDevice;
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

template <typename T>
void*& Void(T*& p)
{
	return reinterpret_cast<void*&>(p);
}

std::pair<HRESULT, StrRaw> Elite::Renderer::InitializeDirectX()
{

	D3D_FEATURE_LEVEL  featureLevel = D3D_FEATURE_LEVEL_11_0;
	constexpr uint32_t createDeviceFlags = DEBUGMODE ? D3D11_CREATE_DEVICE_DEBUG : 0;
	HRESULT result{};

	result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(result))
		return { result, "Failed to create device" };

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), &Void(m_pDXGIFactory));
	if (FAILED(result))
		return { result, "Failed to create DXGI factory" };

	DXGI_SWAP_CHAIN_DESC swapChain{};
	swapChain.BufferDesc.Width = m_Width;
	swapChain.BufferDesc.Height = m_Height;
	swapChain.BufferDesc.RefreshRate.Numerator = 1;
	swapChain.BufferDesc.RefreshRate.Denominator = 60;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChain.SampleDesc.Count = 1;
	swapChain.SampleDesc.Quality = 0;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 1;
	swapChain.Windowed = true;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Flags = 0;

	SDL_SysWMinfo sysWMinfo{};
	SDL_VERSION(&sysWMinfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);
	swapChain.OutputWindow = sysWMinfo.info.win.window;

	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChain, &m_pSwapChain);
	if (FAILED(result))
		return { result, "Failed to create swap chain" };

	D3D11_TEXTURE2D_DESC          depthStencilDesc{};
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result))
		return { result, "Failed to create viewport texture" };

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
		return { result, "Failed to create viewport dept stencil" };

	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &Void(m_pRenderTargetBuffer));
	if (FAILED(result))
		return { result, "Failed to get swapchain buffer" };

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
		return { result, "Failed to create viewport" };

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);

	return { result, "DirectX initialization succeeded" };

}