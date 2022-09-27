#pragma once

#include "DX11Shaders.h"
#include "DX11Effect.h"
#include "JL/JLMesh.h"

class Mesh_DX11 : public JL::Mesh<SHADER::Vertex, SHADER::Index>
{
	using SR = ID3D11ShaderResourceView;
public:

	using Mesh = JL::Mesh<SHADER::Vertex, SHADER::Index>;

	explicit Mesh_DX11(ID3D11Device* pDevice, Mesh&& mesh, bool transparant);

	~Mesh_DX11() noexcept;

	explicit Mesh_DX11    (Mesh_DX11 const&) = delete;
	Mesh_DX11& operator = (Mesh_DX11 const&) = delete;

	explicit Mesh_DX11    (Mesh_DX11 &&) noexcept;
	Mesh_DX11& operator = (Mesh_DX11 &&) noexcept;

	bool IsTransparant() const noexcept { return m_Transparant; }

	void Render( ID3D11DeviceContext* pDeviceContext,
				 SHADER::Culling culling, SHADER::Sampler samplerState, bool transparancy,
				 shader::float4x4 const* pViewTransformation, shader::float3x3 const* pViewRotation, shader::float3 const* pViewOrigin,
				 SR* pDiffuse, SR* pSpecular, SR* pNormal);

private:

	bool				m_Transparant;

	Effect_DX11			m_Effect;

	ID3D11InputLayout*  m_pVertexLayout;

	ID3D11Buffer*	    m_VertexBuffer;
	ID3D11Buffer*       m_IndexBuffer;

	Index				m_IndexCount;

};

