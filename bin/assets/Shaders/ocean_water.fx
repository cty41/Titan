#include "light_scattering_constants.h"

// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;
float4 posOffset : posScaleOffset = {1.0f, 0.0f, 0.0f, 0.0f};
float4 vCameraPos: worldcamerapos;
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

float4 sun_vec: sunvector = 
  {0.578f,0.578f,0.578f,0.0f};
float4 xAxis = 
  {1.0f, 0.0f, 0.0f, 0.0f};
float4 vOne = 
  {1.0f, 1.0f, 1.0f, 0.0f};
float4 vHalf = 
  {0.5f, 0.5f, 0.5f, 0.0f};

float3 waterColor0 = 
  {0.15f, 0.4f, 0.5f};

float3 waterColor1 = 
  {0.1f, 0.15f, 0.3f};

texture tex0 : TEXTURE; // blend mask

struct VS_INPUT
{
	float2	Pos		: POSITION;
	float		ZPos0	: POSITION1;
	float2	Norm0	: NORMAL0;
	float		ZPos1	: POSITION2;
	float2	Norm1	: NORMAL1;
};

struct VS_OUTPUT
{
	float4 Pos  : POSITION;
  float3 Col	: COLOR0;
	float3 T0		: TEXCOORD0;
	float3 T1		: TEXCOORD1;
	float3 T2		: TEXCOORD2;
	float3 T3		: TEXCOORD3;
};

VS_OUTPUT VS(VS_INPUT v)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

	// offset xy and interpolate
	// z to get world position	
	float3 worldPos = 
	float3(
		v.Pos.x+posOffset.z,
		v.Pos.y+posOffset.w,
		lerp(v.ZPos0, v.ZPos1, posOffset.x));

	// transform and output
	Out.Pos = 
		mul(float4(worldPos, 1), mWorldViewProj); 

	// interpolate normal
	float2 nXY = lerp(v.Norm0, v.Norm1, posOffset.x);
	float3 normal = normalize(float3(nXY, 24.0f));

	// compute tex coords using world pos
	// and normal for animation
	float3 uvBase = 
		float3(
			worldPos.x*0.01f, 
			worldPos.y*0.01f, 
			0.0f);

	float3 uvAnim = normal * 0.1f;
	Out.T0 = uvBase + uvAnim;
	Out.T1 = (uvBase * 0.5f) - uvAnim;

	// compute binormal and 
	// tangent using cross products
	float3 tangent = xAxis.yzx *  normal.zxy;
	tangent = (-normal.yzx * xAxis.zxy) + tangent;

	float3 binormal = normal.yzx *  tangent.zxy;
	binormal = (-tangent.yzx * normal.zxy) + binormal;

	// transform the sun vector to texture space
	float3 lightVec;
	lightVec.x = dot(sun_vec, binormal);
	lightVec.y = dot(sun_vec, tangent);
	lightVec.z = dot(sun_vec, normal);

	// normalize the light vector
	// and output
	Out.T2 = normalize(lightVec);

	//compute the view vector
	float3 camera_vec = vCameraPos - worldPos;
	float s = length(camera_vec);
	camera_vec = normalize(camera_vec);

	// transform the view vector to texture space
	float3 viewVec;
	viewVec.x = dot(camera_vec, binormal);
	viewVec.y = dot(camera_vec, tangent);
	viewVec.z = dot(camera_vec, normal);

	// normalize the view vector
	viewVec = normalize(viewVec);

	// compute the half-angle vector
	float3 half_angle_vec = (viewVec + lightVec)*0.5f;

	// normalize the half vector
	// and output
	Out.T3  = normalize(half_angle_vec);

	// color interpolator is the dot product 
	// of the view vector with the normal
	float cosTheta = saturate(dot(-camera_vec, normal));
	Out.Col = lerp(waterColor0, waterColor1, cosTheta);

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

float4 PS(VS_OUTPUT In) : COLOR
{
	// composite bump maps
	float3 bump0 = (tex2D(LinearSamp0, In.T0 )-0.5f)*2.0f; 
	float3 bump1 = (tex2D(LinearSamp0, In.T1 )-0.5f)*2.0f; 
	float3 bump = (bump0+bump1)*0.5f;

	// compute base diffuse color
	float3 baseColor = dot(bump, In.T2) * In.Col;

	// compute specular component
	float specFactor = dot(bump, In.T3);
	float3 specColor = 
		specFactor * specFactor * 
		specFactor * specFactor * sun_color;

	// combine and output
	return float4((baseColor+specColor), 1.0f);
}

technique OceanWater_1_1
{
	pass P0
	{
		//	FILLMODE = WIREFRAME;
		CULLMODE = CW;
		ZENABLE = TRUE;
		ZWRITEENABLE = TRUE;
		ZFUNC = LESSEQUAL;

		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS();
	}  
}
