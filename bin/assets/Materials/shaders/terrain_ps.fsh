float4 terrain_ps(float4 pos: POSITION,
				float2 tex0: TEXCOORD0,
				float2 tex1: TEXCOORD1,
				uniform sampler sampler0: register(s0),
				uniform sampler sampler1: register(s1),
				uniform sampler sampler2: register(s2),
				uniform sampler sampler3: register(s3)): COLOR
{
	float4 blender = tex2D(sampler0, tex0);
	float4 Color0 = tex2D(sampler1, tex1) * blender.r;
	float4 Color1 = tex2D(sampler2, tex1) * blender.g; 
	float4 Color2 = tex2D(sampler3, tex1) * blender.b; 
	return (Color0 + Color1 + Color2) * blender.a;
}