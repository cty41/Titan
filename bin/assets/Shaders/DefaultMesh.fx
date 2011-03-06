// material reflectivity
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse

texture tex0 : TEXTURE;

// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex  : TEXCOORD0;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION,
		float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Pos = mul(float4(Pos, 1), mWorldViewProj);  // position (view space)
		Out.Tex = Tex;

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
    return tex2D(LinearSamp0, In.Tex );
}

technique TVertexAndPixelShader
{
	pass P0
	{
		CULLMODE = CCW;
		ZENABLE = TRUE;
		ZWRITEENABLE = TRUE;
		ZFUNC = LESSEQUAL;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS();
	}  
}
