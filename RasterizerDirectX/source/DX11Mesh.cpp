#include "pch.h"
#include "DX11Mesh.h"
#include "DX11Effect.h"

Mesh_DX11::Mesh_DX11(ID3D11Device* pDevice, Mesh&& mesh, bool transparant)
	: Mesh{ std::move(mesh) }
	, m_Effect{ pDevice, SHADER::FILENAME }
	, m_VertexBuffer{}
	, m_IndexBuffer{}
	, m_IndexCount{}
	, m_pVertexLayout{}
	, m_Transparant{ transparant }
{

	D3DX11_PASS_DESC passDesc;
	m_Effect.GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout(
		data(SHADER::VERTEX_DESC),
		size(SHADER::VERTEX_DESC),
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout
	);

	if (FAILED(result))
	{
		puts("[error] Failed to create input layout\nthrowing...");
		throw std::exception{ "Failed to create input layout" };
	}

	D3D11_BUFFER_DESC      bufferDesc{};
	D3D11_SUBRESOURCE_DATA bufferData{};

	bufferDesc.Usage          = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth      = sizeof(Vertex) * size(vertices);
	bufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags      = 0;
	bufferData.pSysMem        = data(vertices);
	
	result = pDevice->CreateBuffer(
		&bufferDesc,
		&bufferData,
		&m_VertexBuffer
	);

	if (FAILED(result))
	{
		puts("[error] Failed to create vertex buffer\nthrowing...");
		throw std::exception{ "Failed to create vertex buffer" };
	}

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth      = sizeof(Face) * size(faces);
	bufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags      = 0;
	bufferData.pSysMem        = data(faces);

	result = pDevice->CreateBuffer(
		&bufferDesc,
		&bufferData,
		&m_IndexBuffer
	);

	if (FAILED(result))
	{
		puts("[error] Failed to create index buffer\nthrowing...");
		throw std::exception{ "Failed to create index buffer" };
	}

}

Mesh_DX11::~Mesh_DX11() noexcept
{
	if (m_IndexBuffer  ) std::exchange(m_IndexBuffer    , nullptr)->Release();
	if (m_VertexBuffer ) std::exchange(m_VertexBuffer   , nullptr)->Release();
	if (m_pVertexLayout) std::exchange(m_pVertexLayout  , nullptr)->Release();
}

Mesh_DX11::Mesh_DX11(Mesh_DX11&& o) noexcept
	: Mesh{ std::move(static_cast<Mesh&&>(o)) }
	, m_Effect{ std::move(o.m_Effect) }
	, m_Transparant{ o.m_Transparant }
{
	m_pVertexLayout    = std::exchange(o.m_pVertexLayout, nullptr);
	m_VertexBuffer     = std::exchange(o.m_VertexBuffer , nullptr);
	m_IndexBuffer      = std::exchange(o.m_IndexBuffer  , nullptr);
	m_IndexCount       = o.m_IndexCount;
}

Mesh_DX11& Mesh_DX11::operator=(Mesh_DX11&& o) noexcept
{
	Mesh_DX11::~Mesh_DX11();
	static_cast<Mesh&>(*this) = std::move(static_cast<Mesh&&>(o));
	m_Transparant      = o.m_Transparant;
	m_Effect           = std::move(o.m_Effect);
	m_pVertexLayout    = std::exchange(o.m_pVertexLayout, nullptr);
	m_VertexBuffer     = std::exchange(o.m_VertexBuffer , nullptr);
	m_IndexBuffer      = std::exchange(o.m_IndexBuffer  , nullptr);
	m_IndexCount       = o.m_IndexCount;
	return *this;
}

void Mesh_DX11::Render(	ID3D11DeviceContext* pDeviceContext,
						SHADER::Culling culling, SHADER::Sampler samplerState, bool transparancy,
						shader::float4x4 const* pViewTransformation, shader::float3x3 const* pViewRotation, shader::float3 const* pViewOrigin,
						SR* pDiffuse, SR* pSpecular, SR* pNormal)
{
	transparancy &= m_Transparant;

	m_Effect.SetViewTransformation(pViewTransformation);
	m_Effect.SetViewRotation(pViewRotation);
	m_Effect.SetViewOrigin(pViewOrigin);
	
	m_Effect.SetSampleMethod(samplerState);
	m_Effect.SetCullMode(culling);

	m_Effect.SetDiffuseMap(pDiffuse);
	if (!transparancy)
	{
		m_Effect.SetSpecularMap(pSpecular);
		m_Effect.SetNormalMap(pNormal);
	}

	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers    (0, 1, &m_VertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer      (m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetInputLayout      (m_pVertexLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3DX11EffectTechnique* pTechnique;
	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	if (transparancy)
		pTechnique = m_Effect.GetTechniqueTransparancy();
	else
		pTechnique = m_Effect.GetTechnique();

	pTechnique->GetDesc(&techniqueDesc);

	for (size_t i = 0; i < techniqueDesc.Passes; ++i)
	{
		pTechnique->GetPassByIndex(i)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(size(faces) * Face::SIZE, 0, 0);
	}

}