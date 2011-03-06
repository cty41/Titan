//
// Simple Terrain Shader
//
//

#include "light_scattering_constants.h"

// transformations
float4x4 mViewProj: VIEWPROJECTION;
float4 vCameraPos: worldcamerapos;
float4 vCameraNearFar: viewdistances;

float4 posOffset : posScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : uvScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 ambient_light: ambientlight = {0.3f,0.3f,0.6f,0.0f};
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

texture tex0 : TEXTURE; // blend
texture tex1 : TEXTURE; // surface 0
texture tex2 : TEXTURE; // surface 1
texture tex3 : TEXTURE; // surface 2

struct VS_INPUT
{
  float2	Pos		: POSITION0;
  float2	UV		: TEXCOORD0;
	float	ZPos		: POSITION1;
	float3	Norm	: NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vI			: COLOR0;
    float4 vE			: COLOR1;
    float2 vTex0  : TEXCOORD0;
    float2 vTex1  : TEXCOORD1;
    float2 vTex2  : TEXCOORD2;
    float2 vTex3  : TEXCOORD3;
};

VS_OUTPUT VS(const VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

  // transform position immediatly
	float4 worldPos = float4(
		v.Pos.x,
		v.Pos.y,
		v.ZPos,
		1);

	worldPos.xy += posOffset.zw;
	Out.Pos = mul(worldPos, mViewProj);  // position (view space)

  // output texture coords
	Out.vTex0 = (v.UV+texOffset.zw)*texOffset.xy;
	Out.vTex1 = v.UV;
	Out.vTex2 = v.UV;
	Out.vTex3 = v.UV;

	float3 eyeVector = vCameraPos - worldPos; 
	float s = length(eyeVector);
	eyeVector = normalize(eyeVector);

	atmosphericLighting(
		eyeVector,
		sun_vec,
		v.Norm,
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

sampler LinearSamp1 = sampler_state 
{
    texture = <tex1>;
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler LinearSamp2 = sampler_state 
{
    texture = <tex2>;
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler LinearSamp3 = sampler_state 
{
    texture = <tex3>;
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4 ThreeSurfaceBlend(VS_OUTPUT In) : COLOR
{   
	// sample all four textures
	float4 BlendControler = tex2D(LinearSamp0, In.vTex0 );
	float4 texColor0 = tex2D(LinearSamp1, In.vTex1 ); 
	float4 texColor1 = tex2D(LinearSamp2, In.vTex2 ); 
	float4 texColor2 = tex2D(LinearSamp3, In.vTex3 ); 

	// determine the amount of each surface to blend
	float4 Color0 = (texColor0 * BlendControler.r);
	float4 Color1 = (texColor1 * BlendControler.g);
	float4 Color2 = (texColor2 * BlendControler.b);

	// sum the resulting colors 
	// and multiply by the diffuse 
	// vertex color 
	return ((Color0 + Color1 + Color2)*
					(BlendControler.a*In.vE)) + In.vI;
}  


technique SinglePassTerrain
{
    pass P0
    {
			//FILLMODE = WIREFRAME;
			CULLMODE = CW;
			ZENABLE = TRUE;
			ZWRITEENABLE = TRUE;
			ZFUNC = LESSEQUAL;

			AlphaBlendEnable = false;
			AlphaTestEnable = false;

        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 ThreeSurfaceBlend();
    }  
}