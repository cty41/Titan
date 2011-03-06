// transformations
float4x4 mViewProj: VIEWPROJECTION;

float4 posOffset : posScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : uvScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 ambient_light = {0.3f,0.3f,0.6f,0.0f};
float4 sun_vec: sunlight_vec = {0.578f,0.578f,0.578f,0.0f};

texture tex0 : TEXTURE; // blend

struct VS_INPUT
{
  float2	Pos		: POSITION0;
  float2	UV		: TEXCOORD0;
	float	  ZPos	: POSITION1;
	float3	Norm	: NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vDiffuse  : COLOR0;
    float2 vTex0  : TEXCOORD0;
};

VS_OUTPUT VS(const VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4 combinedPos = float4(
		v.Pos.x,
		v.Pos.y,
		v.ZPos,
		1);

	combinedPos.x += posOffset.z;	
	combinedPos.y += posOffset.w;

	Out.Pos = mul(combinedPos, mViewProj);  // position (view space)

	Out.vDiffuse = dot(v.Norm, sun_vec.rgb)+ambient_light;

	Out.vTex0 = (v.UV+texOffset.zw)*texOffset.xy;

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
	// sample the texture
	float4 Color = tex2D(LinearSamp0, In.vTex0 );

	// multiply by the diffuse 
	// vertex color 
	return Color*In.vDiffuse;
}  


technique SinglePassTerrain
{
    pass P0
    {
			FILLMODE = WIREFRAME;
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