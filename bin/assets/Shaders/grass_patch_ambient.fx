#include "patch_deform.h"

// transformations
float4x4 mViewProj: VIEWPROJECTION;
float4x4 mWorld: WORLD;

// the ambient light color
float4 ambient_light = {0.3f,0.3f,0.6f,1.0f};

// the cube map
texture tex0 : TEXTURE;

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
	  float2 tex  : TEXCOORD0;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION,
		float2 tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

		PatchDeform(Pos, mWorld);

    Out.Pos = mul(float4(Pos, 1), mViewProj);  // position (view space)
    Out.tex = tex;

    return Out;
}


sampler LinearSamp0 = 
sampler_state 
{
    texture = <tex0>;
    AddressU  = clamp;        
    AddressV  = clamp;
    AddressW  = clamp;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};


float4 PS(VS_OUTPUT vIN) : COLOR
{
	return tex2D(LinearSamp0, vIN.tex ) * ambient_light; 
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
			AlphaRef = 16;
			AlphaFunc = Greater;

       // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }  
}
