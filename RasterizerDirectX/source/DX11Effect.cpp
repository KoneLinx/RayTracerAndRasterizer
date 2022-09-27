#include "pch.h"
#include "DX11Shaders.h"
#include "DX11Effect.h"
#include <cstdio>

Effect_DX11::Effect_DX11(ID3D11Device* pDevice, std::wstring const file)
	: Effect_DX11{}
{

	ID3D10Blob* pErrorBlob{};

	constexpr DWORD shaderFlags = DEBUGMODE ? D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;

	HRESULT result = D3DX11CompileEffectFromFile(
		file.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&m.pEffect,
		&pErrorBlob
	);

	if (FAILED(result) || !m.pEffect->IsValid())
	{
		puts("[error] Could not load effect file");
		if (pErrorBlob != nullptr)
		{
			puts("Dumping errors:");
			puts(std::string{
				static_cast<char*>(pErrorBlob->GetBufferPointer()),
				pErrorBlob->GetBufferSize()
			}.c_str());
			pErrorBlob->Release();
		}
		else puts("Unknown error");
		puts("Throwing...");
		throw std::exception{ "Failed to load effect file" };
	}

	m.pTechnique.BothPoint        = m.pEffect->GetTechniqueByName("BothPoint");
	m.pTechnique.BothLinear       = m.pEffect->GetTechniqueByName("BothLinear");
	m.pTechnique.BothAnisotropic  = m.pEffect->GetTechniqueByName("BothAnisotropic");
	m.pTechnique.FrontPoint       = m.pEffect->GetTechniqueByName("FrontPoint");
	m.pTechnique.FrontLinear      = m.pEffect->GetTechniqueByName("FrontLinear");
	m.pTechnique.FrontAnisotropic = m.pEffect->GetTechniqueByName("FrontAnisotropic");
	m.pTechnique.BackPoint        = m.pEffect->GetTechniqueByName("BackPoint");
	m.pTechnique.BackLinear       = m.pEffect->GetTechniqueByName("BackLinear");
	m.pTechnique.BackAnisotropic  = m.pEffect->GetTechniqueByName("BackAnisotropic");

	m.pTechniqueTransparancy      = m.pEffect->GetTechniqueByName("Transparancy");
	
	bool anyInvalid =
	std::any_of(
		std::begin(m.pTechniqueArrayFlat), std::end(m.pTechniqueArrayFlat),
		[](auto p)
		{
			return !p->IsValid();
		}
	);

	if (anyInvalid || !m.pTechniqueTransparancy->IsValid())
	{
		puts("[error] Could not load techique\nThorwing...");
		throw std::exception{ "Failed to load technique" };
	}

	m.pVarViewMatrix = m.pEffect->GetVariableBySemantic(SHADER::VAR_VIEW_MATRIX)->AsMatrix();

	if (!m.pVarViewMatrix->IsValid())
	{
		puts("[error] Could not find default view transformation variable\nThorwing...");
		throw std::exception{ "Failed to find view matrix variable" };
	}

	m.pVarViewRotation = m.pEffect->GetVariableBySemantic(SHADER::VAR_VIEW_ROTATION)->AsMatrix();

	if (!m.pVarViewMatrix->IsValid())
	{
		puts("[error] Could not find default view transformation variable\nThorwing...");
		throw std::exception{ "Failed to find view matrix variable" };
	}

	m.pVarViewOrigin = m.pEffect->GetVariableBySemantic(SHADER::VAR_VIEW_ORIGIN)->AsVector();

	if (!m.pVarViewMatrix->IsValid())
	{
		puts("[error] Could not find default view transformation variable\nThorwing...");
		throw std::exception{ "Failed to find view matrix variable" };
	}

	m.pDiffuseMap = m.pEffect->GetVariableBySemantic(SHADER::VAR_DIIFUSE_MAP)->AsShaderResource();

	if (!m.pDiffuseMap->IsValid())
	{
		puts("[error] Could not find default diffuse map variable\nThorwing...");
		throw std::exception{ "Failed to find diffuse map variable" };
	}

	m.pSpecularMap = m.pEffect->GetVariableBySemantic(SHADER::VAR_SPECULAR_MAP)->AsShaderResource();

	if (!m.pDiffuseMap->IsValid())
	{
		puts("[error] Could not find default specular map variable\nThorwing...");
		throw std::exception{ "Failed to find specular map variable" };
	}

	m.pNormalMap = m.pEffect->GetVariableBySemantic(SHADER::VAR_NORMAL_MAP)->AsShaderResource();

	if (!m.pDiffuseMap->IsValid())
	{
		puts("[error] Could not find default normal map variable\nThorwing...");
		throw std::exception{ "Failed to find normal map variable" };
	}

}

Effect_DX11::~Effect_DX11()
{
	if (m.pVarViewMatrix)        std::exchange(m.pVarViewMatrix, nullptr)->Release();
	if (m.pDiffuseMap)           std::exchange(m.pDiffuseMap   , nullptr)->Release();

	for (auto*& pTechnique : m.pTechniqueArrayFlat)
		if (pTechnique)          std::exchange(pTechnique      , nullptr)->Release();

	if (m.pEffect)               std::exchange(m.pEffect       , nullptr)->Release();
}

Effect_DX11::Effect_DX11(Effect_DX11&& other) noexcept
	: Effect_DX11{}
{
	std::swap(_data, other._data);
}

Effect_DX11& Effect_DX11::operator=(Effect_DX11&& other) noexcept
{
	std::swap(_data, other._data);
	return *this;
}

void Effect_DX11::SetViewTransformation(shader::float4x4 const* p)
{
	if (FAILED(m.pVarViewMatrix->SetMatrix(reinterpret_cast<float const*>(p))))
	{
		puts("[error] Failed to update view transformation for effect\nThrowing...");
		throw std::exception{ "Failed to set view matrix in effect" };
	}
}

void Effect_DX11::SetViewRotation(shader::float3x3 const* p)
{
	if (FAILED(m.pVarViewRotation->SetMatrix(reinterpret_cast<float const*>(p))))
	{
		puts("[error] Failed to update view rotation for effect\nThrowing...");
		throw std::exception{ "Failed to set view rotation in effect" };
	}
}

void Effect_DX11::SetViewOrigin(shader::float3 const* p)
{
	if (FAILED(m.pVarViewOrigin->SetFloatVector(reinterpret_cast<float const*>(p))))
	{
		puts("[error] Failed to update view origin for effect\nThrowing...");
		throw std::exception{ "Failed to set view origin in effect" };
	}
}

void Effect_DX11::SetDiffuseMap(ID3D11ShaderResourceView* pResourceView)
{
	if (FAILED(m.pDiffuseMap->SetResource(pResourceView)))
	{
		puts("[error] Failed to update diffuse map for effect\nThrowing...");
		throw std::exception{ "Failed to set diffuse map texture variable" };
	}
}

void Effect_DX11::SetSpecularMap(ID3D11ShaderResourceView* pResourceView)
{
	if (FAILED(m.pSpecularMap->SetResource(pResourceView)))
	{
		puts("[error] Failed to update specular map for effect\nThrowing...");
		throw std::exception{ "Failed to set specular map texture variable" };
	}
}

void Effect_DX11::SetNormalMap(ID3D11ShaderResourceView* pResourceView)
{
	if (FAILED(m.pNormalMap->SetResource(pResourceView)))
	{
		puts("[error] Failed to update normal map for effect\nThrowing...");
		throw std::exception{ "Failed to set normal map texture variable" };
	}
}

void Effect_DX11::SetSampleMethod(SHADER::Sampler state)
{
	switch (state)
	{
	case SHADER::Sampler::POINT:
		m.Sampling = 0;
		return;

	case SHADER::Sampler::LINEAR:
		m.Sampling = 1;
		return;

	case SHADER::Sampler::ANISOTROPIC:
		m.Sampling = 2;
		return;
	}
	puts("[error] An unavailable sampler state was requested\nThrowing...");
	throw std::exception{ "tried to set an unavailable sampling state" };
}

void Effect_DX11::SetCullMode(SHADER::Culling state)
{
	switch (state)
	{
	case SHADER::Culling::BOTH:
		m.Culling = 0;
		return;

	case SHADER::Culling::FRONT:
		m.Culling = 1;
		return;

	case SHADER::Culling::BACK:
		m.Culling = 2;
		return;

	}
	puts("[error] An unavailable cull mode was requested\nThrowing...");
	throw std::exception{ "tried to set an unavailable cull mode" };

}

