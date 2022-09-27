


//	Structures
//

struct VS_INPUT
{
	float3 position	: POSITION;
	float2 uv       : UV;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

struct VS_OUTPUT
{
	float4 position	: SV_POSITION;
    float3 normal   : NORMAL;
    float3 tanegnt  : TANGENT;
	float2 uv       : UV;
};


float3   gViewOrigin         : VIEW_ORIGIN;
float3x3 gViewRotation       : VIEW_ROTATION;
float4x4 gViewTransformation : VIEW_TRANSFORMATION;

Texture2D gDiffuseMap  : DIFFUSE_MAP;
Texture2D gSpecularMap : SPECULAR_MAP;
Texture2D gNormalMap   : NORMAL_MAP;

float3 gLightDirection = -normalize(float3(.577f, -.577f, -.577f));



//  Sampler
//

SamplerState samplerStatePoint
{
    Filter      = MIN_MAG_MIP_POINT;
    AddressU    = Clamp;
    AddressV    = Clamp;
    BorderColor = float4(1.f, 0.f, 1.f, 1.f);
};
SamplerState samplerStateLinear
{
    Filter      = MIN_MAG_MIP_LINEAR;
    AddressU    = Clamp;
    AddressV    = Clamp;
    BorderColor = float4(1.f, 0.f, 1.f, 1.f);
};
SamplerState samplerStateAnisotropic
{
    Filter      = ANISOTROPIC;
    AddressU    = Clamp;
    AddressV    = Clamp;
    BorderColor = float4(1.f, 0.f, 1.f, 1.f);
};



//  Culling
//

RasterizerState rasterizerFront
{
    CullMode = back;
};
RasterizerState rasterizerBack
{
    CullMode = front;
};
RasterizerState rasterizerBoth
{
    CullMode = none;
};


//  Shaders
//

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.f), gViewTransformation);
    output.normal = input.normal;
    output.tanegnt = input.tangent;
    output.uv = input.uv;
	return output;
}


float Phong(float3 view, float3 light, float3 specular, float3 normal)
{
    float reflectance = 3.f;
    float exponent = 0.7f;
    float3 reflection = reflect(light, normal);
    float factor = specular.x * reflectance * pow(max(0, -dot(view, reflection)), exponent);
    return factor;
}

float Lambert(float3 light, float3 normal)
{
    return saturate(dot(normal, light));
}

float3 Shade(VS_OUTPUT pixel, float3 diffuse, float3 specular, float3 normal)
{
    float3 view = normalize(gViewOrigin - pixel.position.xyz);
    
    float3x3 normalTransform = float3x3(
        normalize(cross(pixel.tanegnt, pixel.normal)),
        pixel.tanegnt,
        pixel.normal
    );
    float3 newNormal = mul((normal - float3(.5f, .5f, .5f)) * 2, normalTransform);
    
    float lambert = Lambert(gLightDirection, newNormal);
    
    float3 result = diffuse * lambert * 3.f * (
        Lambert(gLightDirection, newNormal) +
        Phong(view, gLightDirection, specular, newNormal)
    );
     
    return saturate(result);
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return gDiffuseMap.Sample(samplerStatePoint, input.uv);
}
float4 PS_p(VS_OUTPUT input) : SV_Target
{
    float3 result = Shade(
        input,
        gDiffuseMap .Sample(samplerStatePoint, input.uv).xyz,
        gSpecularMap.Sample(samplerStatePoint, input.uv).xyz,
        gNormalMap  .Sample(samplerStatePoint, input.uv).xyz
    );
    return float4(result, 1.f);
}
float4 PS_l(VS_OUTPUT input) : SV_Target
{
    float3 result = Shade(
        input,
        gDiffuseMap .Sample(samplerStateLinear, input.uv).xyz,
        gSpecularMap.Sample(samplerStateLinear, input.uv).xyz,
        gNormalMap  .Sample(samplerStateLinear, input.uv).xyz
    );
    return float4(result, 1.f);
}
float4 PS_a(VS_OUTPUT input) : SV_Target
{
    float3 result = Shade(
        input,
        gDiffuseMap .Sample(samplerStateAnisotropic, input.uv).xyz,
        gSpecularMap.Sample(samplerStateAnisotropic, input.uv).xyz,
        gNormalMap  .Sample(samplerStateAnisotropic, input.uv).xyz
    );
    return float4(result, 1.f);
}


// Technique
//

DepthStencilState depthDefault
{
};
BlendState blendDefault
{
};

technique11 DefaultTechnique
{
	pass P0
	{
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

technique11 BothPoint
{
    pass P0
    {
        SetRasterizerState(rasterizerBoth);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_p()));
    }
}
technique11 BothLinear
{
    pass P0
    {
        SetRasterizerState(rasterizerBoth);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_l()));
    }
}
technique11 BothAnisotropic
{
    pass P0
    {
        SetRasterizerState(rasterizerBoth);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_a()));
    }
}

technique11 FrontPoint
{
    pass P0
    {
        SetRasterizerState(rasterizerFront);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_p()));
    }
}
technique11 FrontLinear
{
    pass P0
    {
        SetRasterizerState(rasterizerFront);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_l()));
    }
}
technique11 FrontAnisotropic
{
    pass P0
    {
        SetRasterizerState(rasterizerFront);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_a()));
    }
}

technique11 BackPoint
{
    pass P0
    {
        SetRasterizerState(rasterizerBack);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_p()));
    }
}
technique11 BackLinear
{
    pass P0
    {
        SetRasterizerState(rasterizerBack);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_l()));
    }
}
technique11 BackAnisotropic
{
    pass P0
    {
        SetRasterizerState(rasterizerBack);
        SetDepthStencilState(depthDefault, 0);
        SetBlendState(blendDefault, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_a()));
    }
}

BlendState blendTransparancy
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};
DepthStencilState depthTransparancy
{
    DepthEnable    = true;
    DepthWriteMask = zero;
    DepthFunc      = less;
    StencilEnable  = false;

    StencilReadMask  = 0x0F;
    StencilWriteMask = 0x0F;

    FrontFaceStencilFunc      = always;
    BackFaceStencilFunc       = always;
    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail  = keep;
    FrontFaceStencilPass      = keep;
    BackFaceStencilPass       = keep;
    FrontFaceStencilFail      = keep;
    BackFaceStencilFail       = keep;
};

technique11 Transparancy
{
    pass P0
    {
        SetRasterizerState(rasterizerBoth);
        SetDepthStencilState(depthTransparancy, 0);
        SetBlendState(blendTransparancy, float4(0,0,0,0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}