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
}