#include "light_scattering_constants.h"

// material reflectivity
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse

texture tex0 : TEXTURE;

// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;
float4x4 mWorld: WORLD;

float4 vCameraPos: worldcamerapos;
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vI			: COLOR0;
    float4 vE			: COLOR1;
    float2 Tex  : TEXCOORD0;
    float4 NdL  : TEXCOORD1;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION,
		float3 Norm  : NORMAL,
		float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

		float4 worldPos = mul(float4(Pos, 1), mWorld);
		float3 worldNorm = mul(float4(Norm, 0), mWorld);

		worldNorm = normalize(worldNorm);
    Out.Pos = mul(float4(Pos, 1), mWorldViewProj);  
		Out.Tex = Tex;

		float ndl = dot(worldNorm, sun_vec);
		Out.NdL = float4(ndl, ndl, ndl, 1.0f);

		float3 eyeVector = vCameraPos - worldPos; 
		float s = length(eyeVector);
		eyeVector = normalize(eyeVector);

		atmosphericLighting(
			eyeVector,
			sun_vec,
			worldNorm,
			sun_color,
			s,
			Out.vE,
			Out.vI);

    return Out;
}

sampler LinearSamp0 = sampler_state 
{
    texture = <tex0>;
    AddressU  = clamp;        
    AddressV  = clamp;
    AddressW  = clamp;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4 PS(VS_OUTPUT In) : COLOR
{
		float4 texColor = tex2D(LinearSamp0, In.Tex );			

		texColor.xyz = texColor.xyz * In.NdL.xyz;

    return (texColor*In.vE) + In.vI;
}

technique TVertexAndPixelShader
{
    pass P0
    {
 		CULLMODE = CCW;
		ZENABLE = TRUE;
		ZWRITEENABLE = TRUE;
		ZFUNC = LESSEQUAL;

			AlphaBlendEnable = false;
			AlphaTestEnable = true;
			AlphaRef = 127;
			AlphaFunc = Greater;

       // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }  
}
