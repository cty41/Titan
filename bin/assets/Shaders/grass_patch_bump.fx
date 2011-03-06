#include "patch_deform.h"

// transformations
float4x4 mViewProj: VIEWPROJECTION;
float4x4 mWorld: WORLD;

float4 vCameraPos: worldcamerapos;
float4 vCameraNearFar: viewdistances;
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

float4 zAxis = 
  {0.0f, 0.0f, -1.0f, 0.0f};
float4 vOne = 
  {1.0f, 1.0f, 1.0f, 0.0f};
float4 vHalf = 
  {0.5f, 0.5f, 0.5f, 0.0f};

// the cube map
texture tex0 : TEXTURE;
texture tex1 : TEXTURE;

struct VS_OUTPUT
{
    float4 Pos			 : POSITION;
	  float2 tex0			 : TEXCOORD0;
	  float2 tex1			 : TEXCOORD1;
    float4 vLightVec : TEXCOORD2;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION,
		float3 Norm  : NORMAL,
		float2 tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

		PatchDeform(Pos, mWorld);

    Out.Pos = mul(float4(Pos, 1), mViewProj);  // position (view space)
    Out.tex0 = tex;
    Out.tex1 = tex;

		// compute the world normal
		float4 worldNorm = mul(float4(Norm, 0), mWorld);
		worldNorm = normalize(worldNorm);

		// compute the cross product of the world
		// normal and the zAxis vector. This is the xAxis
		// of the polygon face
		float3 xAxis = zAxis.yzx *  worldNorm.zxy;
		xAxis = (-worldNorm.yzx * zAxis.zxy) + xAxis;

		// transform the light vector to texture space
		float4 lightVec;
		lightVec.x = dot(sun_vec, xAxis);
		lightVec.y = dot(sun_vec, zAxis);
		lightVec.z = dot(sun_vec, worldNorm);
		lightVec.w = 0.0f;

		// normalize the light vector
		lightVec = normalize(lightVec);

		// output as 0-1 value
		Out.vLightVec  = (lightVec+vOne)*vHalf;

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

sampler LinearSamp1 = 
sampler_state 
{
    texture = <tex1>;
    AddressU  = clamp;        
    AddressV  = clamp;
    AddressW  = clamp;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4 PS(VS_OUTPUT In) : COLOR
{
	float4 bump= (tex2D(LinearSamp0, In.tex0 )-0.5f)*2.0f; 
	float4 alpha= tex2D(LinearSamp1, In.tex1 ); 
	float3 svec= (In.vLightVec-0.5f)*2.0f;

	float4 result = saturate(dot(bump, svec)) * alpha.w;
	
	return result;
}

technique GrassPatchBump
{
  pass P0
  {
		CULLMODE = CCW;
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
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
