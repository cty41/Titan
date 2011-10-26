#include "TitanStableHeader.h"
#include "TiResourceGroupMgr.h"
#include "TiResourceMgr.h"
#include "TiFileSystemMgr.h"
#include "TiLogMgr.h"
#include "TiScriptLoader.h"

namespace Titan
{
	String	ResourceGroupMgr::GENERAL_RESOURCE_GROUP = "General";
	String	ResourceGroupMgr::INTERNAL_RESOURCE_GROUP = "Internal";
	void ResourceGroupMgr::ResourceGroup::insertFileLocation(const String& name, const String& relativePath, FileSystem* fileSystem)
	{
		FileLocationMap::iterator it = fileLocationMap.find(name);
		if(it == fileLocationMap.end())
		{
			fileLocationMap.insert(FileLocationMap::value_type(name, TITAN_NEW LocationInfo(relativePath, fileSystem)));
		}
		else
		{
			TITAN_EXCEPT_INTERNALERROR(
				"There has a file with same name: " + name + "  exists in  group " + groupName);
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
			TITAN_EXCEPT_INTERNALERROR(
				"There has not a group with  name: " + group);
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
			TITAN_EXCEPT_INTERNALERROR(
				"There has a group with  name: " + group);
		}
		//cache the res group
		if(mCurrentGroup != gp)
			mCurrentGroup = gp;
		return gp;
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::parseResourceGroup(ResourceGroup* rg)
	{
		LogMgr::getSingleton().stream()<<"Begin Parsing resource group named: " + rg->groupName;

		ScriptLoaderToFileLocationMap scriptLoaderToFileLocationMap;

		//first search all the file, and bind the scripts to its ScriptLoader
		ScriptLoaderMap::iterator sit = mScriptLoaderMap.begin(), sitEnd = mScriptLoaderMap.end();
		while (sit != sitEnd)
		{
			FileLocationMapPtr fileLocationMapPtr = FileLocationMapPtr(TITAN_NEW_T(FileLocationMap, MEMCATEGORY_GENERAL), SPFM_DELETE_T);
			FileLocationMap::iterator fit = rg->fileLocationMap.begin(), fitEnd = rg->fileLocationMap.end();
			while(fit != fitEnd)
			{
				StringVector exts = sit->second->getScriptExts();
				StringVector::iterator eit = exts.begin(), eitEnd = exts.end();
				while(eit != eitEnd)
				{
					size_t lastDotIdx = fit->first.find_last_of('.');
					//if the file has no ext, continue
					if(lastDotIdx == String::npos)
						continue;
					int cmpResult = fit->first.compare(lastDotIdx, (*eit).length(), *eit);
					if(!cmpResult)
					{
						fileLocationMapPtr->insert(FileLocationMap::value_type(fit->first,fit->second));
						break;
					}
					++eit;
				}
				++fit;
			}
			scriptLoaderToFileLocationMap.insert(ScriptLoaderToFileLocationMap::value_type(sit->second, fileLocationMapPtr));
			++sit;
		}

		//parse all the script
		ScriptLoaderToFileLocationMap::iterator  slit = scriptLoaderToFileLocationMap.begin(), slitEnd = scriptLoaderToFileLocationMap.end();
		while(slit != slitEnd)
		{
			ScriptLoader* sl = slit->first;
			FileLocationMap::iterator fit = slit->second->begin(), fitEnd = slit->second->end();
			while(fit != fitEnd)
			{
				DataStreamPtr dataStream = fit->second->pFileSystem->open(fit->second->relativePath+fit->first);
				sl->parseScript(dataStream, rg->groupName);
				++fit;
			}
			++slit;
		}

		LogMgr::getSingleton().stream()<<"Finished Parsing resource group named: " + rg->groupName;
	}
	//-------------------------------------------------------------------------------//
	void ResourceGroupMgr::initResourceGroup(const String& group)
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end() &&
			it->second->state == ResourceGroup::RGS_UNINITIALISED)
		{
			it->second->state = ResourceGroup::RGS_INITIALISING;
			//parse the scripts if has any
			parseResourceGroup(it->second);
			it->second->state = ResourceGroup::RGS_INITIALISED;
			mCurrentGroup = it->second;
		}
		else
		{
			TITAN_EXCEPT_ITEMLOST(
				"We does not have a resource group called : " + group);
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
			//parse the scripts if has any
			parseResourceGroup(it->second);
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
			TITAN_EXCEPT_ITEMLOST(
				" the resource group called" + group + " does not exist!");
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
			FileLocationMap::iterator fit = rg->fileLocationMap.begin(), fitEnd = rg->fileLocationMap.end();
			while(fit != fitEnd)
			{
				TITAN_DELETE fit->second;
				++fit;
			}

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
			TITAN_EXCEPT_ITEMLOST(
				" the resource group called" + group + " does not exist!");
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
			TITAN_EXCEPT_ITEMLOST(
				" the resource group called" + group + " does not exist!");
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
				DataStreamPtr dataPtr = fit->second->pFileSystem->open(fit->second->relativePath + name);
				return dataPtr;
			}
			
		}

		TITAN_EXCEPT_ITEMLOST(
			" the resource called" + name + "in group " + group + " does not exist!");
	
	}
	//------------------------------------------------------------------------------//
	void ResourceGroupMgr::_registerScriptLoader(ScriptLoader* su)
	{
		ScriptLoaderMap::iterator it = mScriptLoaderMap.find(su->getScriptLoaderPriority());
		if(it != mScriptLoaderMap.end())
		{
			TITAN_EXCEPT_INTERNALERROR(
				"the script loader with same priority has existed!");
		}
		else
		{
			mScriptLoaderMap.insert(ScriptLoaderMap::value_type(su->getScriptLoaderPriority(), su));
		}
	}
	//------------------------------------------------------------------------------//
	void ResourceGroupMgr::_unregisterScriptLoader(ScriptLoader* su)
	{
		ScriptLoaderMap::iterator it = mScriptLoaderMap.find(su->getScriptLoaderPriority());
		if(it == mScriptLoaderMap.end())
		{
			TITAN_EXCEPT_INTERNALERROR(
				"the script loader has not add to resource group mgr!");
		}
		else
		{
			mScriptLoaderMap.erase(su->getScriptLoaderPriority());
		}
	}
}