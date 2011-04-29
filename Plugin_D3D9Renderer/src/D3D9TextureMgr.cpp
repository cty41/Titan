#include "D3D9TextureMgr.h"
#include "D3D9Texture.h"

namespace Titan
{
	D3D9TextureMgr::D3D9TextureMgr()
		:TextureMgr()
	{

	}
	//-------------------------------------------------------------//
	D3D9TextureMgr::~D3D9TextureMgr()
	{

	}
	//-------------------------------------------------------------//
	Resource* D3D9TextureMgr::createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams)
	{
		D3D9Texture* pTex = TITAN_NEW D3D9Texture(this, name, id, group, isManual);
		return pTex;
	}
	//-------------------------------------------------------------//
}