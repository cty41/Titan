// transformations
float4x4 mViewProj: VIEWPROJECTION;

// terrain section offsets
float4 posOffset : posScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : uvScaleOffset = {1.0, 1.0, 0.0f, 0.0f};

// texture maps used
texture tex0 : TEXTURE; // blend
texture tex1 : TEXTURE; // surface 0
texture tex2 : TEXTURE; // surface 1
texture tex3 : TEXTURE; // surface 2

// the ambient light color
float4 ambient_light = {0.3f,0.3f,0.6f,0.0f};

struct VS_INPUT
{
  float2	Pos		: POSITION0;
  float2	UV		: TEXCOORD0;
	float	ZPos		: POSITION1;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
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

	Out.Pos = mul(combinedPos, mViewProj);

	Out.vTex0 =
		(v.UV+texOffset.zw)*texOffset.xy;
	Out.vTex1 = v.UV;
	Out.vTex2 = v.UV;
	Out.vTex3 = v.UV;

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
	// and multiply by the ambient 
	// light color 
	return (Color0 + Color1 + Color2)
		*BlendControler.a * ambient_light;
}  

//
// This technique outputs ambient color
// while filling the z buffer with 
// depth values
//
technique AmbientTerrainShader2_0
{
    pass P0
    {
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

technique AmbientTerrainShader1_1
{
    pass P0
    {
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