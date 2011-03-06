// transformations
float4x4 mViewProj: VIEWPROJECTION;

float4 posOffset : posScaleOffset = 
  {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : uvScaleOffset = 
  {1.0, 1.0, 0.0f, 0.0f};
float4 ambient_light = 
  {0.3f,0.3f,0.6f,0.0f};
float4 sun_vec: sunvector = 
  {0.578f,0.578f,0.578f,0.0f};
float4 xAxis = 
  {1.0f, 0.0f, 0.0f, 0.0f};
float4 vOne = 
  {1.0f, 1.0f, 1.0f, 0.0f};
float4 vHalf = 
  {0.5f, 0.5f, 0.5f, 0.0f};

texture tex0 : TEXTURE; // blend mask
texture tex1 : TEXTURE; // normal map 0
texture tex2 : TEXTURE; // normal map 1
texture tex3 : TEXTURE; // normal map 2

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
    float3 vLightVec  : COLOR0;
    float2 vTex0  : TEXCOORD0;
    float2 vTex1  : TEXCOORD1;
    float2 vTex2  : TEXCOORD2;
    float2 vTex3  : TEXCOORD3;
};

VS_OUTPUT VS(const VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4 combinedPos = float4(
		v.Pos.x,
		v.Pos.y,
		v.ZPos,
		1);

	combinedPos.xy += posOffset.zw;

	Out.Pos = mul(combinedPos, mViewProj);  // position (view space)

	Out.vTex0 = (v.UV+texOffset.zw)*texOffset.xy;
	Out.vTex1 = v.UV;
	Out.vTex2 = v.UV;
	Out.vTex3 = v.UV;

	// compute binormal and 
	// tangent using cross products
	float3 tangent = xAxis.yzx *  v.Norm.zxy;
	tangent = (-v.Norm.yzx * xAxis.zxy) + tangent;

	float3 binormal = v.Norm.yzx *  tangent.zxy;
	binormal = (-tangent.yzx * v.Norm.zxy) + binormal;

	// transform the light vector to texture space
	float4 lightVec;
	lightVec.x = dot(sun_vec, binormal);
	lightVec.y = dot(sun_vec, tangent);
	lightVec.z = dot(sun_vec, v.Norm);
	lightVec.w = 0.0f;

	// normalize the light vector
	lightVec = normalize(lightVec);

	// output as 0-1 value
	Out.vLightVec  = (lightVec+vOne)*vHalf;

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

// Pixel Shader 1_x is incapable of
// normalizing a vector. We are
// relying on the fact that the mask
// texture has been setup to allow only
// one of the bump map textures to appear
// on screen - preventing the need to 
// renormalize the surface normals
float4 PS_1x(VS_OUTPUT In) : COLOR
{   
	// sample all four textures
	float4 mask = tex2D(LinearSamp0, In.vTex0 ); 
	float4 texColor0 = tex2D(LinearSamp1, In.vTex1 ); 
	float4 texColor1 = tex2D(LinearSamp2, In.vTex2 ); 
	float4 texColor2 = tex2D(LinearSamp3, In.vTex3 ); 

	// determine the amount of each surface to blend
	float4 Color0 = (texColor0 * mask.r);
	float4 Color1 = (texColor1 * mask.g);
	float4 Color2 = (texColor2 * mask.b);

	float4 normal = Color0+Color1+Color2;

	// compute the per-pixel dot product
	// and output the result
	return saturate(dot(
		(In.vLightVec-0.5f)*2.0f, 
		(normal-0.5f)*2.0f));
}  

// Pixel Shader 2_x is capable of
// normalizing a vector. We can use
// a regular blend texture here and
// normalize the result prior to
// performing the dot product
float4 PS_2x(VS_OUTPUT In) : COLOR
{   
	// sample all four textures
	float4 mask = tex2D(LinearSamp0, In.vTex0 ); 
	float4 texColor0 = tex2D(LinearSamp1, In.vTex1 ); 
	float4 texColor1 = tex2D(LinearSamp2, In.vTex2 ); 
	float4 texColor2 = tex2D(LinearSamp3, In.vTex3 ); 

	// determine the amount of each surface to blend
	float4 Color0 = (texColor0 * mask.r);
	float4 Color1 = (texColor1 * mask.g);
	float4 Color2 = (texColor2 * mask.b);

	float4 normal = normalize(Color0+Color1+Color2);

	// compute the per-pixel dot product
	// and output the result
	return saturate(dot(
		(In.vLightVec-0.5f)*2.0f, 
		(normal-0.5f)*2.0f));
}  


technique SinglePassBumpTerrain2_0
{
  pass P0
  {
		CULLMODE = CW;
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		ZFUNC = LESSEQUAL;

		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS_1x();
  }  
}


technique NormalizingSinglePassBumpTerrain
{
  pass P0
  {
		CULLMODE = CW;
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		ZFUNC = LESSEQUAL;

		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS_2x();
  }  
}

technique SinglePassBumpTerrain1_4
{
  pass P0
  {
		CULLMODE = CW;
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		ZFUNC = LESSEQUAL;

		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS_1x();
  }  
}

technique SinglePassBumpTerrain1_1
{
  pass P0
  {
		CULLMODE = CW;
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		ZFUNC = LESSEQUAL;

		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS_1x();
  }  
}

