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
		removeAll();
	}
	//-------------------------------------------------------------//
	ResourceMgr::CreatedResource ResourceMgr::create(const String& name, const String& group, AnyMap* extraParams /* = 0 */)
	{
		//if this resource has been created
		ResourceMap::iterator it = mResourceMap.find(name);
		if(it != mResourceMap.end())
		{
			return CreatedResource(false, it->second);
		}
		else
		{
			ResourceHandle id = getNextHandle();
			Resource* res= createImpl(name, id, group, extraParams);
			ResourcePtr pRes(res);
			//add to 
			addImpl(pRes);

			ResourceGroupManager::getSingltonPtr()->addCreatedResource(group, pRes);
			return CreatedResource(true, pRes);
		}

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
	ResourcePtr ResourceMgr::load(const String& name, const String& group, AnyMap* extraParams)
	{
		CreatedResource created = create(name, group, extraParams);

		created.second->load();

		return created.second;
	}
	//-------------------------------------------------------------//
	void ResourceMgr::unload(const String& name)
	{
		ResourceMap::iterator it = mResourceMap.find(name);
		if(it != mResourceMap.end())
		{
			it->second->unload();
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"The resource called" + name + " does not found in" + mType,
				"ResourceMgr::unload");
		}
	}
	//-------------------------------------------------------------//
	void ResourceMgr::unload(ResourceHandle ResourceID)
	{
		ResourceHandleMap::iterator it = mResourceHandleMap.find(ResourceID);
		if(it != mResourceHandleMap.end())
		{
			it->second->unload();
		}
		else
		{
			StringStream errMsg;
			errMsg<<ResourceID;
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"The resource with ID " + errMsg.str() + " does not found in" + mType,
				"ResourceMgr::unload");
		}
	}
	//-------------------------------------------------------------//
	void ResourceMgr::unloadAll()
	{
		ResourceMap::iterator it = mResourceMap.begin(), itend = mResourceMap.end();
		while(it != itend)
		{
			it->second->unload();
			++it;
		}
	}
	//-------------------------------------------------------------//
	void ResourceMgr::unloadUnreferencedResources()
	{

	}
	//-------------------------------------------------------------//
	void ResourceMgr::addImpl(ResourcePtr& res)
	{
		//to add into map
		mResourceMap.insert(ResourceMap::value_type(res->getName(), res));
		mResourceHandleMap.insert(ResourceHandleMap::value_type(res->getID(), res));
	}
}