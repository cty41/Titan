// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;

// the cube map
texture tex0 : TEXTURE;
texture tex1 : TEXTURE;

float4 uv_data: uvScaleOffset = {1.0f,1.0f,0.0f,0.0f};

struct VS_INPUT
{
  float4	Pos	: POSITION;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vTex0: TEXCOORD0;
    float2 vTex1: TEXCOORD1;
    float2 vTex2: TEXCOORD2;
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
	Out.vTex0.xyz = normalize(v.Pos.yzx);
	Out.vTex0.w=1.0f;

	Out.vTex1 = v.Pos.xy+uv_data.zw;
	Out.vTex2 = (v.Pos.xy+uv_data.xy)*3.7f;

  return Out;
}

// cube map coordinates should not wrap
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
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};


float4 CloudShader(VS_OUTPUT In) : COLOR
{   
	// sample the cube map and clouds
	float4 background= tex3D(LinearSamp0, In.vTex0 ); 
	float4 cloud0= tex2D(LinearSamp1, In.vTex1 ); 
	float4 cloud1= tex2D(LinearSamp1, In.vTex2 ); 
	
	// modulate the clouds together and
	// mask with the background alpha
	float4 cloud_layer = 
		cloud0 * cloud1.a * background.a;

	// sum and return the results
	return background + cloud_layer;
}  

technique BasicCubeMap
{
    pass P0
    {
		//FILLMODE = WIREFRAME;
		FILLMODE = SOLID;

		// no culling
		CULLMODE = NONE;

		ZENABLE = TRUE;
		ZWRITEENABLE = TRUE;
		ZFUNC = always;

		// also clear any stencil
		StencilEnable = true;
		StencilFunc = always;
		StencilPass = replace;
		StencilRef = 0;

			AlphaBlendEnable = false;
			AlphaTestEnable = false;

        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 CloudShader();
    }
		
	  
}

