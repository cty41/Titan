texture tex0 : TEXTURE;

// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;
float4x4 mWorld: WORLD;

float4 vCameraPos: worldcamerapos;
float4 vViewVector: worldviewvector;
float4 ambient_light = {0.3f,0.3f,0.6f,1.0f};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex  : TEXCOORD0;
    float4 VdL  : TEXCOORD1;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION,
		float3 Norm  : NORMAL,
		float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Pos = mul(float4(Pos, 1), mWorldViewProj); 
		Out.Tex = Tex;

		float4 worldPos = mul(float4(Pos, 1), mWorld);
		float2 norm2d = mul(float4(Norm, 0), mWorld);
		norm2d = normalize(norm2d);

		float2 eyeVector = -vViewVector; 
		eyeVector = normalize(eyeVector);
		float view = dot(norm2d, eyeVector);
		view = sign(view-0.85);
		Out.VdL = float4(view, view, view, 1.0f);

    return Out;
}

sampler LinearSamp0 = sampler_state 
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
		float4 texColor = tex2D(LinearSamp0, In.Tex )* ambient_light;			

		texColor.w = texColor.w * In.VdL.z;

    return texColor;
}

technique TVertexAndPixelShader
{
    pass P0
    {
 		   CULLMODE = CCW;
		   ZENABLE = TRUE;
		   ZWRITEENABLE = TRUE;
		   ZFUNC = LESSEQUAL;

			 AlphaBlendEnable = false;
			 AlphaTestEnable = true;
			 AlphaRef = 16;
			 AlphaFunc = Greater;

       // shaders
       VertexShader = compile vs_2_0 VS();
       PixelShader  = compile ps_2_0 PS();
    }  
}
