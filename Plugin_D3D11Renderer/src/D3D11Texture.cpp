#include "D3D11Texture.h"

namespace Titan
{
	D3D11Texture::D3D11Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual)
		: Texture(mgr, name, id, group, isManual)
	{
	}
	//-------------------------------------------------------------//
	D3D11Texture::~D3D11Texture()
	{
	}
}