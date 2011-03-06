#include "TitanStableHeader.h"
#include "TitanTexture.h"

namespace Titan
{
	Texture::Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
		: Resource(mgr, name, id, group)
	{
	}
	//-------------------------------------------------------------//
	Texture::~Texture()
	{

	}
	//-------------------------------------------------------------//
	void Texture::generatePerlinNoise(float scale, int octaves, float falloff)
	{
		if(mType == TT_2D)
		{
			_perlinNoiseImpl(scale, octaves, falloff);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"Perlin noise can only made in 2d texture!",
				"Texture::generatePerlinNoise");
		}

	}
}