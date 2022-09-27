#include "pch.h"
#include "JLTextures.h"

JL::SurfaceManager::SurfaceManager() noexcept
	: m_SurfaceMap{}
{}

bool NullTerminated(std::string_view const& str)
{
	auto const last = str.data() + str.size();
	return last[0] == '\0' || last[-1] == '\0';
}

JL::SurfaceManager::SurfaceManager(ID3D11Device* pDevice, std::initializer_list<SurfacePair> files)
	: SurfaceManager{}
{
	for (auto const& file : files)
		if (NullTerminated(file))
			m_SurfaceMap.emplace(file.value, Load(pDevice, file));
		else
			throw std::exception{ "File name string not null terminated." };
}

JL::SurfaceManager::~SurfaceManager() noexcept
{
	for (auto [k, s] : m_SurfaceMap)
	{
		if (s.pSDLSurface)
			SDL_FreeSurface(s.pSDLSurface);
		if (s.pDXResourceView)
			s.pDXResourceView->Release();
		if (s.pDXTexture2DResource)
			s.pDXTexture2DResource->Release();
	}
	m_SurfaceMap.clear();
}

JL::SurfaceManager::Surface emptySurface{};

JL::SurfaceManager::Surface& JL::SurfaceManager::operator[](SurfaceMap::key_type const id)
{
	if (id == 0) return emptySurface;
	return m_SurfaceMap.at(id);
}

JL::SurfaceManager::Surface JL::SurfaceManager::operator[](SurfaceMap::key_type const id) const
{
	if (id == 0) return emptySurface;
	return m_SurfaceMap.at(id);
}


JL::SurfaceManager::SurfacePair JL::SurfaceManager::GenID(std::string_view str) noexcept
{
	constexpr std::hash<std::string_view> hasher{};
	return { { hasher(str) }, std::move(str) };
}

JL::SurfaceManager::Surface JL::SurfaceManager::Load(ID3D11Device* pDevice, std::string_view file) const
{
	Surface surface{};
	SDL_Surface*& pSDLSurface = surface.pSDLSurface;

	pSDLSurface = IMG_Load(data(file));

	if (pSDLSurface == nullptr)
	{
		puts("[error] SDL could not load surface.\nThrowing...");
		throw std::exception{ "SDL Surface could not be loaded" };
	}

	D3D11_TEXTURE2D_DESC ResourceDesc{};

	ResourceDesc.MipLevels          = 1;
	ResourceDesc.ArraySize          = 1;
	ResourceDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	ResourceDesc.SampleDesc.Count   = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Usage              = D3D11_USAGE_DEFAULT;
	ResourceDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	ResourceDesc.CPUAccessFlags     = 0;
	ResourceDesc.MiscFlags          = 0;
	
	ResourceDesc.Width              = pSDLSurface->w;
	ResourceDesc.Height             = pSDLSurface->h;

	D3D11_SUBRESOURCE_DATA data{};

	data.pSysMem                    =                    pSDLSurface->pixels;
	data.SysMemPitch                = static_cast<UINT>( pSDLSurface->pitch                  );
	data.SysMemSlicePitch           = static_cast<UINT>( pSDLSurface->pitch * pSDLSurface->h );

	HRESULT result = pDevice->CreateTexture2D(&ResourceDesc, &data, &surface.pDXTexture2DResource);

	if (FAILED(result))
	{
		puts("[error] Failed to create texture resource.\nThrowing...");
		throw std::exception{ "Failed to create texture resource." };
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ViewDesc{};
	ViewDesc.Format                 = ResourceDesc.Format;
	ViewDesc.ViewDimension          = D3D11_SRV_DIMENSION_TEXTURE2D;
	ViewDesc.Texture2D.MipLevels    = 1;

	result = pDevice->CreateShaderResourceView(surface.pDXTexture2DResource, &ViewDesc, &surface.pDXResourceView);

	if (FAILED(result))
	{
		puts("[error] Failed to create shader resource view.\nThrowing...");
		throw std::exception{ "Failed to create shader resource view." };
	}

	return surface;

}
