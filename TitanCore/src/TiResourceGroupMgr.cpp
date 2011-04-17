#include "TitanStableHeader.h"
#include "TiResourceGroupMgr.h"
#include "TiResourceMgr.h"
#include "TiFileSystemMgr.h"


namespace Titan
{
	String	ResourceGroupMgr::GENERAL_RESOURCE_GROUP = "General";
	String	ResourceGroupMgr::INTERNAL_RESOURCE_GROUP = "Internal";
	void ResourceGroupMgr::ResourceGroup::insertFileLocation(const String& name, const String& relativePath, FileSystem* fileSystem)
	{
		FileLocationMap::iterator it = fileLocationMap.find(name);
		if(it == fileLocationMap.end())
		{
			fileLocationMap.insert(FileLocationMap::value_type(name, LocationInfo(relativePath, fileSystem)));
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"There has a file with same name: " + name + "  exists in  group " + groupName,
				"ResourceGroup::insertFileLocation");
		}
	}

	template<> ResourceGroupMgr* Singleton<ResourceGroupMgr>::ms_Singleton = 0;

	ResourceGroupMgr* ResourceGroupMgr::getSingltonPtr()
	{
		return ms_Singleton;
	}
	ResourceGroupMgr& ResourceGroupMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	ResourceGroupMgr::ResourceGroupMgr()
		:mCurrentGroup(0)
	{
		createResourceGroup(INTERNAL_RESOURCE_GROUP);
		createResourceGroup(GENERAL_RESOURCE_GROUP);

	}
	//-------------------------------------------------------------------------------//
	ResourceGroupMgr::~ResourceGroupMgr()
	{
		destroyAllResourceGroup();
	}
	//-------------------------------------------------------------------------------//
	FileSystem* ResourceGroupMgr::addResourceLocation(const String& name, const String& type, const String& group, bool recursive)
	{
		ResourceGroup* gp;
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end())
		{
			gp = it->second;
		}
		else
		{
			gp = TITAN_NEW ResourceGroup();
			mResourceGroupMap.insert(ResourceGroupMap::value_type(group, gp));
			gp->state = ResourceGroup::RGS_UNINITIALISED;
			gp->groupName = group;
		}

		FileSystem* fileSystem = FileSystemManager::getSingletonPtr()->load(name, type);
		StringVectorPtr pStringVector, pPathsVector;
		fileSystem->find("*", recursive, &pStringVector, &pPathsVector);
		StringVector::iterator sit = (pStringVector)->begin(), sitend = (pStringVector)->end();
		StringVector::iterator sit2 = (pPathsVector)->begin(), sitend2 = (pPathsVector)->end();
		while (sit != sitend && sit2 != sitend2)
		{
			gp->insertFileLocation(*sit, *sit2, fileSystem);
			++sit, ++sit2;
		}

		return fileSystem;
	}
	//-------------------------------------------------------------------------------//
	ResourceGroupMgr::ResourceGroup* ResourceGroupMgr::getResourceGroup(const String& group)
	{
		ResourceGroup* gp = 0;
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end())
		{
			gp = it->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"There has not a group with  name: " + group,
				"ResourceGroupMgr::getResourceGroup");
		}

		return gp;

	}
	//-------------------------------------------------------------------------------//
	ResourceGroupMgr::ResourceGroup* ResourceGroupMgr::createResourceGroup(const String& group)
	{
		ResourceGroup* gp = 0;
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it == mResourceGroupMap.end())
		{
			gp = TITAN_NEW ResourceGroup();
			mResourceGroupMap.insert(ResourceGroupMap::value_type(group, gp));
			gp->state = ResourceGroup::RGS_UNINITIALISED;
			gp->groupName = group;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"There has a group with  name: " + group,
				"ResourceGroupMgr::createResourceGroup");
		}
		//cache the res group
		if(mCurrentGroup != gp)
			mCurrentGroup = gp;
		return gp;
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::parseResourceGroup(ResourceGroup* rg)
	{
		//now here is useless, 'cause we do not have the script -cty
#if 0
		ResourceOrderListMap::iterator it = rg->resourceOrderListMap.begin(),
			itend = rg->resourceOrderListMap.end();
		while(it != itend)
		{

			while()
			++it;
		}
#endif
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::initResourceGroup(const String& group)
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end() &&
			it->second->state == ResourceGroup::RGS_UNINITIALISED)
		{
			it->second->state = ResourceGroup::RGS_INITIALISING;
			//if we need to implement script
			//add content here
			parseResourceGroup(it->second);
			it->second->state = ResourceGroup::RGS_INITIALISED;
			mCurrentGroup = it->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"We does not have a resource group called : " + group,
				"ResourceGroupMgr::initResourceGroup");
		}
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::initAllResourceGroup()
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.begin(), itend = mResourceGroupMap.end();
		while(it != itend &&
			it->second->state == ResourceGroup::RGS_UNINITIALISED)
		{
			it->second->state = ResourceGroup::RGS_INITIALISING;
			//if we need to implement script
			//add content here
			it->second->state = ResourceGroup::RGS_INITIALISED;
			++it;
		}
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::destroyResourceGroup(const String& group)
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end())
		{
			ResourceGroup *rg = it->second;
			ResourceOrderListMap::iterator lit = rg->resourceOrderListMap.begin(), litend = rg->resourceOrderListMap.end();
			while(lit != litend)
			{
				TITAN_DELETE_T(lit->second++, ResourceList, MEMCATEGORY_GENERAL);
			}
			
			TITAN_DELETE it->second;
			mResourceGroupMap.erase(it);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				" the resource group called" + group + " does not exist!",
				"ResourceGroupMgr::destroyResourceGroup");
		}

	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::destroyAllResourceGroup()
	{
		//maybe got memory leak here -cty
		ResourceGroupMap::iterator it = mResourceGroupMap.begin(), itend = mResourceGroupMap.end();
		while(it != itend)
		{
			ResourceGroup *rg = it->second;
			ResourceOrderListMap::iterator lit = rg->resourceOrderListMap.begin(), litend = rg->resourceOrderListMap.end();
			while(lit != litend)
			{
				TITAN_DELETE_T(lit->second, ResourceList, MEMCATEGORY_GENERAL);
				++lit;
			}

			TITAN_DELETE it->second;
			mResourceGroupMap.erase(it++);
		}
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::loadResourceGroup(const String& group)
	{
		//change later , we need to ad resource analyzer, maybe add in init
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end())
		{
			ResourceGroup *rg = it->second;
			ResourceOrderListMap::iterator lit = rg->resourceOrderListMap.begin(), litend = rg->resourceOrderListMap.end();
			while(lit != litend)
			{
				ResourceList::iterator resit = lit->second->begin(), resitend = lit->second->end();
				while(resit++ != resitend)
				{
					(*resit)->load();
				}
			}
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				" the resource group called" + group + " does not exist!",
				"ResourceGroupMgr::loadResourceGroup");
		}
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::unloadResourceGroup(const String& group)
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end())
		{
			ResourceGroup *rg = it->second;
			ResourceOrderListMap::iterator lit = rg->resourceOrderListMap.begin(), litend = rg->resourceOrderListMap.end();
			while(lit != litend)
			{
				ResourceList::iterator resit = lit->second->begin(), resitend = lit->second->end();
				while(resit++ != resitend)
				{
					(*resit)->unload();
				}
			}
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				" the resource group called" + group + " does not exist!",
				"ResourceGroupMgr::loadResourceGroup");
		}
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::addCreatedResource(const String& group, ResourcePtr& res)
	{
		ResourceGroup* rg;
		if(mCurrentGroup->groupName == group)
		{
			rg = mCurrentGroup;
		}
		else
		{
			ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
			if(it != mResourceGroupMap.end())
			{
				rg = it->second;
			}
			else
			{
				rg = createResourceGroup(group);
			}
		}

		int order = res->getCreator()->getLoadOrder();
		ResourceOrderListMap::iterator it = rg->resourceOrderListMap.find(order);
		if(it != rg->resourceOrderListMap.end())
		{
			it->second->push_back(res);
		}
		else
		{
			rg->resourceOrderListMap[order] = TITAN_NEW_T(ResourceList, MEMCATEGORY_GENERAL)();
			rg->resourceOrderListMap[order]->push_back(res);
		}
	}
	//-------------------------------------------------------------------------------//
	DataStreamPtr ResourceGroupMgr::openResource(const String& name, const String& group)
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end())
		{
			FileLocationMap::iterator fit = it->second->fileLocationMap.find(name);
			if(fit != it->second->fileLocationMap.end())
			{
				DataStreamPtr dataPtr = fit->second.pFileSystem->open(fit->second.relativePath + name);
				return dataPtr;
			}
			
		}

		TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
			" the resource called" + name + "in group " + group + " does not exist!",
			"ResourceGroupMgr::openResource");
	
	}

}