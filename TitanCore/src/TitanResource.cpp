#include "TitanStableHeader.h"
#include "TitanResource.h"
#include "TitanResourceGroupManager.h"
#include "TitanResourceManager.h"

namespace Titan
{
	Resource::Resource(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
	: mMgr(mgr), mName(name), mID(id), mGroup(group), mLoadState(LS_UNLOAD)
	{
		
	}
	//-------------------------------------------------------------//
	Resource::~Resource()
	{

	}
	//-------------------------------------------------------------//
	void Resource::load()
	{
		if(!isLoaded())
		{
			mLoadState = LS_LOADING;
			loadImpl();
			mLoadState = LS_LOADED;
		}
	}
	//-------------------------------------------------------------//
	void Resource::unload()
	{
		if(isLoaded())
		{
			unloadImpl();
			mLoadState = LS_UNLOAD;
		}
	}
	//-------------------------------------------------------------//
	void Resource::changeGroup(const String& newGroup)
	{
	}
}