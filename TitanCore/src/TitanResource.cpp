#include "TitanStableHeader.h"
#include "TitanResource.h"
#include "TitanResourceGroupManager.h"
#include "TitanResourceManager.h"

namespace Titan
{
	Resource::Resource(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
	: mMgr(mgr), mName(name), mID(id), mGroup(group)
	{

	}
	//-------------------------------------------------------------//
	Resource::~Resource()
	{

	}
	//-------------------------------------------------------------//
	void Resource::load()
	{

	}
	//-------------------------------------------------------------//
	void Resource::unload()
	{

	}
	//-------------------------------------------------------------//
	void Resource::changeGroup(const String& newGroup)
	{

	}
}