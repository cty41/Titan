#include "light_scattering_constants.h"

// transformations
float4x4 mWorldViewProj: WORLDVIEWPROJECTION;
float4x4 mWorldView: WORLDVIEW;
float4x4 mWorld: WORLD;
float4 vCameraPos: worldcamerapos;
float4 vCameraNearFar: viewdistances;

float4 ambient_light = {0.3f,0.3f,0.6f,0.0f};
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

// the cube map
texture tex0 : TEXTURE;
texture tex1 : TEXTURE;

struct VS_INPUT
{
  float4	Pos	: POSITION;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vI			: COLOR0;
    float4 vE			: COLOR1;
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

	// eye vector and view distance s
	float3 eyeVector = normalize(v.Pos.xyz);
	float s = 3000.0f;

	// compute angle and phase1 theta
	float cosTheta = dot(eyeVector.rgb, -sun_vec.rgb);
	float p1Theta = (cosTheta*cosTheta)+atm.vConstants.x;

	// compute extinction term E
	// -(beta_1+beta_2) * s * log_2 e
	float4 E = -atm.vSumBeta1Beta2 * s * atm.vConstants.y;
	E.x = exp(E.x);
	E.y = exp(E.y);
	E.z = exp(E.z);

	// compute phase2 theta as
  // (1-g^2)/(1+g-2g*cos(theta))^(3/2)
	// notes:
	// theta is 180 - actual theta (this corrects for sign)
	// atm.vHG = [1-g^2, 1+g, 2g]
	float p2Theta = (atm.vHG.z*cosTheta)+atm.vHG.y;
	p2Theta = 1.0f/(sqrt(p2Theta));
	p2Theta = (p2Theta*p2Theta*p2Theta) * atm.vHG.x;

	// compute inscattering (I) as
	//(Beta_1 * Phase_1(theta) + Beta_2 * Phase_2(theta)) * 
	//[1-exp(-Beta_1*s).exp(-Beta_2*s)] / (Beta_1 + Beta_2)
	//
	// or, more simply
	//
	// (vBetaD1*p1Theta + vBetaD1*p2Theta) *
	// (1-E) * atm.vRcpSumBeta1Beta2
	// 
	float4 I = (atm.vBetaD1*p1Theta)+
						(atm.vBetaD2*p2Theta);

	I = I*(atm.vConstants.x-E)*atm.vRcpSumBeta1Beta2;

	// scale inscatter and extinction (optional)
	I = saturate(I*atm.vTermMultipliers.x);
	E = saturate(E*atm.vTermMultipliers.y);//*atm.vSoilReflectivity;

	float falloff = s * vCameraNearFar.w;
	falloff = falloff * falloff *falloff*falloff;

	// apply sunlight color
	// and strength to each term
	// and output
	Out.vI.xyz = saturate(I)*(sun_color*sun_color.w);
	Out.vI.w = falloff;
	Out.vE = E*sun_color*sun_color.w;//*(1-falloff);

	return Out;
}

float4 CloudShader(VS_OUTPUT In) : COLOR
{   
	return In.vI;
}  

technique BasicCubeMap
{
    pass P0
    {
			//FILLMODE = WIREFRAME;
			FILLMODE = SOLID;

			// no culling
			CULLMODE = NONE;

			ZENABLE = false;
			ZWRITEENABLE = false;

			AlphaBlendEnable = false;
			AlphaTestEnable = false;

        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 CloudShader();
    }
		
	  
}

