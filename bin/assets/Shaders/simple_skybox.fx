// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;

// the cube map
texture tex0 : TEXTURE;

struct VS_INPUT
{
  float4	Pos	: POSITION;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vTex0: TEXCOORD0;
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
	// to convert the verts to 
	// 3D texture coords
	Out.vTex0 = v.Pos.yzxw;

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

float4 CubeMap(VS_OUTPUT In) : COLOR
{   
	// return the cube map texture
	return tex3D(LinearSamp0, In.vTex0 ); 
}  

technique BasicCubeMap
{
    pass P0
    {
			//FILLMODE = WIREFRAME;

			// no culling
			CULLMODE = NONE;

			AlphaBlendEnable = false;
			AlphaTestEnable = false;

			ZENABLE = TRUE;
			ZWRITEENABLE = TRUE;
			ZFUNC = always;

			// also clear any stencil
			StencilEnable = true;
			StencilFunc = always;
			StencilPass = replace;
			StencilRef = 0;


      // shaders
      VertexShader = compile vs_2_0 VS();
      PixelShader  = compile ps_2_0 CubeMap();
    }  
}

