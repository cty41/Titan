// transformations
float4x4 mViewProj: VIEWPROJECTION;

float4 posOffset : posScaleOffset = {1.0, 1.0, 0.0f, 0.0f};
float4 texOffset : uvScaleOffset = {1.0, 1.0, 0.0f, 0.0f};

float4 ambient_light = {0.3f,0.3f,0.6f,0.0f};
float4 sun_vec: sunlight_vec = {0.578f,0.578f,0.578f,0.0f};

texture tex0 : TEXTURE; // blend
texture tex1 : TEXTURE; // surface 0
texture tex2 : TEXTURE; // surface 1
texture tex3 : TEXTURE; // surface 2
texture tex4 : TEXTURE; // surface 3
texture tex5 : TEXTURE; // noise channels

struct VS_INPUT
{
  float2	Pos		: POSITION0;
  float2	UV		: TEXCOORD0;
	float	ZPos		: POSITION1;
	float3	Norm	: NORMAL;
};

struct VS_OUTPUT11
{
    float4 Pos  : POSITION;
    float4 vDiffuse  : COLOR0;
    float2 vTex0  : TEXCOORD0;
    float2 vTex1  : TEXCOORD1;
    float2 vTex2  : TEXCOORD2;
    float2 vTex3  : TEXCOORD3;
};

struct VS_OUTPUT14
{
    float4 Pos  : POSITION;
    float4 vDiffuse  : COLOR0;
    float2 vTex0  : TEXCOORD0;
    float2 vTex1  : TEXCOORD1;
    float2 vTex2  : TEXCOORD2;
    float2 vTex3  : TEXCOORD3;
    float2 vTex4  : TEXCOORD4;
    float2 vTex5  : TEXCOORD5;
};

VS_OUTPUT11 VS11(const VS_INPUT v)
{
	VS_OUTPUT11 Out = (VS_OUTPUT11)0;

	float4 combinedPos = float4(
		v.Pos.x,
		v.Pos.y,
		v.ZPos,
		1);

	combinedPos.xy += posOffset.zw;

	Out.Pos = mul(combinedPos, mViewProj);  // position (view space)

	Out.vDiffuse = dot(v.Norm, sun_vec.rgb) + ambient_light;

	Out.vTex0 = (v.UV+texOffset.zw)*texOffset.xy;
	Out.vTex1 = v.UV*2.0f;
	Out.vTex2 = v.UV*2.0f;
	Out.vTex3 = v.UV*0.7f; // tile the noise UVs at a different rate

    return Out;
}

VS_OUTPUT14 VS14(const VS_INPUT v)
{
	VS_OUTPUT14 Out = (VS_OUTPUT14)0;

	float4 combinedPos = float4(
		v.Pos.x,
		v.Pos.y,
		v.ZPos,
		1);

	combinedPos.xy += posOffset.zw;

	Out.Pos = mul(combinedPos, mViewProj);  // position (view space)

	Out.vDiffuse = dot(v.Norm, sun_vec.rgb)+ ambient_light;

	Out.vTex0 = (v.UV+texOffset.zw)*texOffset.xy;
	Out.vTex1 = v.UV;
	Out.vTex2 = v.UV;
	Out.vTex3 = v.UV;
	Out.vTex4 = v.UV;
	Out.vTex5 = v.UV*1.7f; // tile the noise UVs at a different rate

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

sampler LinearSamp4 = sampler_state 
{
    texture = <tex4>;
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler LinearSamp5 = sampler_state 
{
    texture = <tex5>;
    AddressU  = wrap;        
    AddressV  = wrap;
    AddressW  = wrap;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4 FourSurfaceBlend(VS_OUTPUT14 In) : COLOR
{   
	// sample all four textures
	float4 BlendControler = tex2D(LinearSamp0, In.vTex0 );
	float4 texColor0 = tex2D(LinearSamp1, In.vTex1 ); 
	float4 texColor1 = tex2D(LinearSamp2, In.vTex2 ); 
	float4 texColor2 = tex2D(LinearSamp3, In.vTex3 ); 
	float4 texColor3 = tex2D(LinearSamp4, In.vTex4 ); 
	float4 noise = tex2D(LinearSamp5, In.vTex5 ); 

	// apply our noise to the blend
	BlendControler = BlendControler*noise;

	// determine the amount of each surface to blend
	float4 Color0 = (texColor0 * BlendControler.r);
	float4 Color1 = (texColor1 * BlendControler.g);
	float4 Color2 = (texColor2 * BlendControler.b);
	float4 Color3 = (texColor3 * BlendControler.a);

	// sum the resulting colors 
	// and multiply by the diffuse 
	// vertex color 
	return (Color0 + Color1 + Color2 + Color3)*In.vDiffuse;
}  

float4 TwoSurfacePass0(VS_OUTPUT11 In) : COLOR
{ 
	const  float4 vC0 =float4(1.0f,0.0f,0.0f,0.0f); 
	const  float4 vC1 =float4(0.0f,0.0f,1.0f,0.0f); 

	float4 t0 = tex2D(LinearSamp0, In.vTex0 );
	float4 t1 = tex2D(LinearSamp1, In.vTex1 ); 
	float4 t2 = tex2D(LinearSamp3, In.vTex2 ); 
	float4 t3 = tex2D(LinearSamp5, In.vTex3 ); 

	t2= t2* dot((t0* t3), vC1);
	t1= t1* dot((t0* t3), vC0);
  
	return (t1+ t2)* In.vDiffuse;
} 

float4 TwoSurfacePass1(VS_OUTPUT11 In) : COLOR
{ 
	const  float4 vC0 =float4(0.0f,1.0f,0.0f,0.0f); 
	const  float4 vC1 =float4(0.0f,0.0f,0.0f,1.0f); 

	float4 t0 = tex2D(LinearSamp0, In.vTex0 );
	float4 t1 = tex2D(LinearSamp2, In.vTex1 ); 
	float4 t2 = tex2D(LinearSamp4, In.vTex2 ); 
	float4 t3 = tex2D(LinearSamp5, In.vTex3 ); 

	t1= t1* dot((t0*t3), vC0);
	t2= t2* dot((t0*t3), vC1);
  
	return (t1+ t2)* In.vDiffuse;
} 

technique MultiPassTerrain
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
      VertexShader = compile vs_2_0 VS11();
      PixelShader = compile ps_2_0 TwoSurfacePass0();
    }  

    pass P1
    {
			CULLMODE = CW;
			ZENABLE = TRUE;
			ZWRITEENABLE = FALSE;
			ZFUNC = LESSEQUAL;

			AlphaTestEnable = false;
			AlphaBlendEnable = true;
			SrcBlend = one;
			DestBlend = one;
			BlendOp = add;

      // shaders
      VertexShader = compile vs_2_0 VS11();
      PixelShader = compile ps_2_0 TwoSurfacePass1();
    }  
}

technique SinglePassTerrain
{
    pass P0
    {
			//FILLMODE = WIREFRAME;
			CULLMODE = CW;
			ZENABLE = TRUE;
			ZWRITEENABLE = TRUE;
			ZFUNC = LESSEQUAL;

			AlphaBlendEnable = false;

        // shaders
        VertexShader = compile vs_2_0 VS14();
        PixelShader  = compile ps_2_0 FourSurfaceBlend();
    }  
}

