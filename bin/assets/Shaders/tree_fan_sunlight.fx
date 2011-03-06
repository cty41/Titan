#include "light_scattering_constants.h"

// material reflectivity
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse

texture tex0 : TEXTURE;

// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;
float4x4 mWorld: WORLD;

float4 vCameraPos: worldcamerapos;
float4 vViewVector: worldviewvector;
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vI			: COLOR0;
    float4 vE			: COLOR1;
    float2 Tex  : TEXCOORD0;
    float4 NdL  : TEXCOORD1;
    float4 VdL  : TEXCOORD2;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION,
		float3 Norm  : NORMAL,
		float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Pos = mul(float4(Pos, 1), mWorldViewProj); 
		Out.Tex = Tex;

		float4 worldPos = mul(float4(Pos, 1), mWorld);
		float3 worldNorm = mul(float4(Norm, 0), mWorld);
		worldNorm = normalize(worldNorm);

		float3 viewVector = vCameraPos - worldPos; 
		float s = length(viewVector);

		float ndl = dot(worldNorm, sun_vec);
		Out.NdL = float4(ndl, ndl, ndl, 1.0f);

		atmosphericLighting(
			normalize(viewVector),
			sun_vec,
			worldNorm,
			sun_color,
			s,
			Out.vE,
			Out.vI);

		// compute the 2D dot product of the world normal
		// with the viewing direction. The biased sign of this
		// value determines if the polygon is visible
		// by producing a 0 or 1 value
		float2 norm2d = worldNorm;
		float2 eyeVector = -vViewVector; 
		norm2d = normalize(norm2d);
		eyeVector = normalize(eyeVector);
		float view = dot(norm2d, eyeVector);
		view = sign(view-0.85);
		Out.VdL = float4(view, view, view, 1.0f);

    return Out;
}

sampler LinearSamp0 = sampler_state 
{
    texture = <tex0>;
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4 PS(VS_OUTPUT vIN) : COLOR
{
	float4 color= tex2D(LinearSamp0, vIN.Tex ); 

	color.xyz = ((color*vIN.vE) + (vIN.vI));
	color.w = color.w * vIN.VdL.z;

	return color;

}

technique TVertexAndPixelShader
{
    pass P0
    {
			CULLMODE = CCW;
			ZENABLE = TRUE;
			ZWRITEENABLE = FALSE;
			ZFUNC = LESSEQUAL;

			AlphaTestEnable = true;
			AlphaRef = 16;
			AlphaFunc = Greater;

			AlphaBlendEnable = true;
			SrcBlend = destAlpha;
			DestBlend = one;
			BlendOp = add;

			// shaders
			VertexShader = compile vs_2_0 VS();
			PixelShader  = compile ps_2_0 PS();
    }  
}
