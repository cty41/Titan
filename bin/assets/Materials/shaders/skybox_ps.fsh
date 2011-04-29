float4 skybox_ps(float3 dir: TEXCOORD0,
				uniform sampler envSampler: register(s0)) :COLOR
{
	return texCUBE(envSampler, dir);
}
