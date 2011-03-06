// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;
float4x4 mWorld: WORLD;

float4 vCameraPos: worldcamerapos;
float4 vCameraNearFar: viewdistances;
float4 vViewVector: worldviewvector;
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

float4 zAxis = 
  {0.0f, 0.0f, -1.0f, 0.0f};
float4 vOne = 
  {1.0f, 1.0f, 1.0f, 0.0f};
float4 vHalf = 
  {0.5f, 0.5f, 0.5f, 0.0f};

// the cube map
texture tex0 : TEXTURE;

struct VS_OUTPUT
{
    float4 Pos			 : POSITION;
	  float2 Tex			 : TEXCOORD0;
    float4 VdL			 : TEXCOORD1;
    float4 vLightVec : TEXCOORD2;
};

VS_OUTPUT VSg(
    float3 Pos  : POSITION,
		float3 Norm  : NORMAL,
		float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Pos = mul(float4(Pos, 1), mWorldViewProj); 
		Out.Tex = Tex;

		// compute the world normal
		float4 worldNorm = mul(float4(Norm, 0), mWorld);
		worldNorm = normalize(worldNorm);

		// compute the cross product of the world
		// normal and the zAxis vector. This is the xAxis
		// of the polygon face
		float3 xAxis = zAxis.yzx *  worldNorm.zxy;
		xAxis = (-worldNorm.yzx * zAxis.zxy) + xAxis;

		// transform the light vector to texture space
		float4 lightVec;
		lightVec.x = dot(sun_vec, xAxis);
		lightVec.y = dot(sun_vec, zAxis);
		lightVec.z = dot(sun_vec, worldNorm);
		lightVec.w = 0.0f;

		// normalize the light vector
		lightVec = normalize(lightVec);

		// output as 0-1 value
		Out.vLightVec  = (lightVec+vOne)*vHalf;

		// compute the 2D dot product of the world normal
		// with the viewing direction. The biased sign of this
		// value determines if the polygon is visible
		// by producing a 0 or 1 value
		float2 norm2d = worldNorm;
		float2 eyeVector = -vViewVector; 
		norm2d = normalize(norm2d);
		eyeVector = normalize(eyeVector);
		float view = dot(norm2d, eyeVector);
		view = sign(view-0.85);
		Out.VdL = float4(view, view, view, 1.0f);


   return Out;
}


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
	float4 bump = tex2D(LinearSamp0, In.Tex );
	float3 normal= (bump-0.5f)*2.0f; 
	float3 svec= (In.vLightVec-0.5f)*2.0f;

  return saturate(dot(normal, svec) * bump.w * In.VdL.z);
}

technique PineTreeBump
{
  pass P0
  {
		CULLMODE = CCW;
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		ZFUNC = LESSEQUAL;

		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		AlphaRef = 16;
		AlphaFunc = Greater;

		// shaders
		VertexShader = compile vs_2_0 VSg();
		PixelShader  = compile ps_2_0 PS();
  }  
}
