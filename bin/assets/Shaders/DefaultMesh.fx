//texture tex0 : Texture;

// transformations
float4x4 mWorldViewProj: WorldViewProj;

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
};

VS_OUTPUT VS(float3 Pos  : POSITION)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Pos = mul(mWorldViewProj, float4(Pos, 1));  // position (view space)

    return Out;
}

//sampler LinearSamp0 = sampler_state 
//{
//   texture = <tex0>;
//   AddressU  = clamp;        
//    AddressV  = clamp;
//    AddressW  = clamp;
//    MIPFILTER = LINEAR;
//    MINFILTER = LINEAR;
//    MAGFILTER = LINEAR;
//};

float4 PS(VS_OUTPUT In) : COLOR
{
    return float4(1.0, 1.0, 1.0, 1.0);
}

technique TVertexAndPixelShader
{
	pass P0
	{
		FILLMODE = WIREFRAME;
		CULLMODE = CW;
		ZENABLE = TRUE;
		ZWRITEENABLE = TRUE;
		ZFUNC = LESSEQUAL;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS();
	}  
}
