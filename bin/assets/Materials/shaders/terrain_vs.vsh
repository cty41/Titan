void terrain_vs(float2	inPos : POSITION0,
				float2 	inUV: TEXCOORD0,
				float  	inHeight: POSITION1,
				uniform float4x4 WorldViewProj,
				uniform float4 uvScaleOffset,
				out float4 oPos : POSITION,
				out float2 oTex0: TEXCOORD0,
				out float2 oTex1: TEXCOORD1)
{
	oPos = mul(float4(inPos.x, inHeight, inPos.y, 1.0), WorldViewProj);
	oTex0 = (inUV + uvScaleOffset.zw) * uvScaleOffset.xy;
	oTex1 = inUV;
}