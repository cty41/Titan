// the flare texture grid
texture tex0 : TEXTURE;

float4 posOffset : posScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : uvScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};
float4 flare_color: flareColor = {1.0f,0.0f,0.0f,1.0f};

struct VS_INPUT
{
  float2	Pos	: POSITION;
  float2	Tex	: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vTex0: TEXCOORD0;
    float4 Diff: COLOR0;
};

VS_OUTPUT VS(const VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	Out.Pos.xy = (v.Pos.xy*posOffset.xy)+ posOffset.zw;
	Out.Pos.z = 0.5f;
	Out.Pos.w = 1.0f;

	Out.vTex0.xy = v.Tex.xy + texOffset.zw;
	Out.vTex0.z = 0.0f;
	Out.vTex0.w = 1.0f;

	Out.Diff = flare_color;
  return Out;
}

// cube map coordinates should not wrap
sampler LinearSamp0 = 
sampler_state 
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
	float4 color= tex2D(LinearSamp0, In.vTex0 ); 
	color = color * In.Diff;
	return color;
}  

technique LensFlare
{
    pass P0
    {
			//FILLMODE = WIREFRAME;

			// no culling
			CULLMODE = NONE;

			AlphaBlendEnable = true;
			SrcBlend = SrcAlpha;
			DestBlend = one;
			BlendOp = add;
			AlphaTestEnable = false;

			// do not test z,
			ZENABLE = FALSE;
			ZWRITEENABLE = FALSE;

      // shaders
      VertexShader = compile vs_2_0 VS();
      PixelShader  = compile ps_2_0 PS();
    }  
}

