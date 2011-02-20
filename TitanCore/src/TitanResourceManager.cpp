#include "TitanStableHeader.h"
#include "TitanResourceManager.h"
#include "TitanResourceGroupManager.h"

namespace Titan
{
	ResourceMgr::ResourceMgr()
		:mNextHandle(0)
	{

	}
	//-------------------------------------------------------------//
	ResourceMgr::~ResourceMgr()
	{

	}
	//-------------------------------------------------------------//
	ResourcePtr ResourceMgr::create(const String& name, const String& group, AnyMap* extraParams /* = 0 */)
	{
		ResourceHandle id = getNextHandle();
		Resource* res= createImpl(name, id, group, extraParams);
		ResourcePtr pRes(res);
		addImpl(&pRes);

		ResourceGroupManager::getSingltonPtr()->addCreatedResource(group, *res);

		return pRes;
	}
	//-------------------------------------------------------------//
	void ResourceMgr::remove(const String& name)
	{
		ResourceMap::iterator it = mResourceMap.find(name);
		if(it != mResourceMap.end())
		{
			it->second.setNull();
			mResourceMap.erase(it);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"the resource called" + name + " does not exist in " + mType,"ResourceMgr::remove" );
		}
	}
	//-------------------------------------------------------------//
	void ResourceMgr::remove(ResourceHandle ResourceID)
	{
		ResourceHandleMap::iterator it = mResourceHandleMap.find(ResourceID);
		if(it != mResourceHandleMap.end())
		{
			it->second.setNull();
			mResourceHandleMap.erase(it);
		}
		else
		{
			StringStream resID;
			resID<<ResourceID;
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"the resource with this ID:" + resID.str() + " does not exist in " + mType,"ResourceMgr::remove" );
		}
	}
	//-------------------------------------------------------------//
	void ResourceMgr::removeAll()
	{
		ResourceHandleMap::iterator it = mResourceHandleMap.begin(), itend = mResourceHandleMap.end();
		while (it != itend)
		{
			it->second.setNull();
			mResourceHandleMap.erase(it++);
		}

		mResourceMap.clear();
	}
	//-------------------------------------------------------------//
	void ResourceMgr::removeUnreferencedResources()
	{
		ResourceHandleMap::iterator it = mResourceHandleMap.begin(), itend = mResourceHandleMap.end();
		while (it != itend)
		{
			if(it->second.isNull())
			{
				mResourceHandleMap.erase(it++);
			}
		}

		ResourceMap::iterator it2 = mResourceMap.begin(), itend2 = mResourceMap.end();
		while (it2 != itend2)
		{
			if(it2->second.isNull())
			{
				mResourceMap.erase(it2++);
			}
		}
	}
	//-------------------------------------------------------------//
	void ResourceMgr::load(const String& name)
	{
		
	}
	//-------------------------------------------------------------//
	void ResourceMgr::load(ResourceHandle ResourceID)
	{

	}
	//-------------------------------------------------------------//
	void ResourceMgr::unload(const String& name)
	{

	}
	//-------------------------------------------------------------//
	void ResourceMgr::unload(ResourceHandle ResourceID)
	{

	}
	//-------------------------------------------------------------//
	void ResourceMgr::unloadAll()
	{

	}
	//-------------------------------------------------------------//
	void ResourceMgr::unloadUnreferencedResources()
	{

	}
	//-------------------------------------------------------------//
	void ResourceMgr::addImpl(ResourcePtr* res)
	{
		
	}


}