#include "D3D11TextureMgr.h"
#include "D3D11Texture.h"

namespace Titan
{
	D3D11TextureMgr::D3D11TextureMgr()
		:TextureMgr()
	{

	}
	//-------------------------------------------------------------//
	D3D11TextureMgr::~D3D11TextureMgr()
	{

	}
	//-------------------------------------------------------------//
	Resource* D3D11TextureMgr::createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams)
	{
		D3D11Texture* pTex = TITAN_NEW D3D11Texture(this, name, id, group, isManual);
		return pTex;
	}
	//-------------------------------------------------------------//
}