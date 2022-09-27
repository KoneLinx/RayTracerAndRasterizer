#pragma once


class Effect_DX11
{

	Effect_DX11() = default;

public:

	explicit Effect_DX11(ID3D11Device* pDevice, std::wstring const file);
	
	~Effect_DX11();

	explicit Effect_DX11    (Effect_DX11 const&) = delete;
	Effect_DX11& operator = (Effect_DX11 const&) = delete;

	explicit Effect_DX11    (Effect_DX11 &&) noexcept;
	Effect_DX11& operator = (Effect_DX11 &&) noexcept;

	auto GetEffect    () noexcept { return m.pEffect; };
	auto GetTechnique () noexcept { return m.pTechniqueArray[m.Culling][m.Sampling]; };
	auto GetTechniqueTransparancy () noexcept { return m.pTechniqueTransparancy; };

	auto* operator -> () noexcept { return GetEffect(); };

	void SetViewTransformation(shader::float4x4 const* pFloat4x4);
	void SetViewRotation      (shader::float3x3 const* pFloat3x3);
	void SetViewOrigin        (shader::float3   const* pFloat3  );

	void SetDiffuseMap        (ID3D11ShaderResourceView* pResourceView);
	void SetSpecularMap       (ID3D11ShaderResourceView* pResourceView);
	void SetNormalMap         (ID3D11ShaderResourceView* pResourceView);
	
	void SetSampleMethod      (SHADER::Sampler state);
	void SetCullMode          (SHADER::Culling state);

private:

	union {

		struct {

			ID3DX11Effect* pEffect;

			int Culling;
			int Sampling;

			union
			{
				struct
				{
					ID3DX11EffectTechnique* BothPoint;
					ID3DX11EffectTechnique* BothLinear;
					ID3DX11EffectTechnique* BothAnisotropic;
					ID3DX11EffectTechnique* FrontPoint;
					ID3DX11EffectTechnique* FrontLinear;
					ID3DX11EffectTechnique* FrontAnisotropic;
					ID3DX11EffectTechnique* BackPoint;
					ID3DX11EffectTechnique* BackLinear;
					ID3DX11EffectTechnique* BackAnisotropic;
				} pTechnique;
				ID3DX11EffectTechnique* pTechniqueArray[3][3];
				ID3DX11EffectTechnique* pTechniqueArrayFlat[9];
			};

			ID3DX11EffectTechnique*      pTechniqueTransparancy;

			ID3DX11EffectMatrixVariable* pVarViewMatrix;
			ID3DX11EffectMatrixVariable* pVarViewRotation;
			ID3DX11EffectVectorVariable* pVarViewOrigin;

			ID3DX11EffectShaderResourceVariable* pDiffuseMap;
			ID3DX11EffectShaderResourceVariable* pSpecularMap;
			ID3DX11EffectShaderResourceVariable* pNormalMap;
		
		} m;

		char _data[sizeof(m)];

	};

};