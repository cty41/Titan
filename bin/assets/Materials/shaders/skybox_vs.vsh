void skybox_vs(float3 inPos: POSITION,
				uniform float4x4 mvpMat,
				out float4 oPos: POSITION,
				out float3 oDir: TEXCOORD0)
{
	oPos = mul(float4(inPos.xyz, 1.0), mvpMat);
	oDir = inPos;
}
