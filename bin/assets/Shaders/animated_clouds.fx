// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;

// the cube map
texture tex0 : TEXTURE;

float4 uv_data: uvScaleOffset = {1.0f,1.0f,0.0f,0.0f};
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};

struct VS_INPUT
{
  float4	Pos	: POSITION;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
		float4 Color: COLOR0;
		float4 Color1: COLOR1;
    float2 vTex0: TEXCOORD0;
    float2 vTex1: TEXCOORD1;
};

VS_OUTPUT VS(const VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// transform the vert
	float4 pos = mul(v.Pos, mWorldViewProj);

	// output with z equal to w
	// to force to farthest possible
	// z value into the depth buffer
	Out.Pos = pos.xyww;

	// a slight swizzle is nessesary
	// along with normaliztion
	// to convert the verts to 
	// 3D texture coords

	Out.vTex0 = v.Pos.xy+uv_data.zw;
	Out.vTex1 = (v.Pos.xy+uv_data.xy)*1.7f;

	Out.Color = sun_vec.z;
	Out.Color1 = saturate(1.0-v.Pos.z);

  return Out;
}

// cube map coordinates should not wrap
sampler LinearSamp0 = 
sampler_state 
{
    texture = <tex0>;
    AddressU  = wrap;        
    AddressV  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};



float4 CloudShader(VS_OUTPUT In) : COLOR
{   
	// sample the cube map and clouds
	float4 cloud0= tex2D(LinearSamp0, In.vTex0 ); 
	float4 cloud1= tex2D(LinearSamp0, In.vTex1 ); 
	
	// modulate the clouds together and
	// mask with the background alpha
	float4 cloud_layer = 
		(cloud0 * cloud1.a) +In.Color1.a;

	// return the results
	return (cloud_layer.a * In.Color);
}  

technique AnimatedClouds
{
	pass P0
	{
		//FILLMODE = WIREFRAME;
		FILLMODE = SOLID;

		// no culling
		CULLMODE = NONE;

		// do not test z,
		ZENABLE = FALSE;
		ZWRITEENABLE = FALSE;

		AlphaBlendEnable = true;
		SrcBlend = srcAlpha;
		DestBlend = one;
		BlendOp = add;

		AlphaTestEnable = false;

		// shaders
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 CloudShader();
	}
}

