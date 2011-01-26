#include "TitanStableHeader.h"
#include "TitanResourceGroupManager.h"
#include "FileSystemManager.h"

namespace Titan
{
	String	ResourceGroupManager::GENERAL_RESOURCE_GROUP = "General";
	void ResourceGroupManager::ResourceGroup::insertFileLocation(const String& name, FileSystem* fileSystem)
	{
		FileLocationMap::iterator it = fileLocationMap.find(name);
		if(it == fileLocationMap.end())
		{
			fileLocationMap.insert(FileLocationMap::value_type(name, fileSystem));
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"There has a file with same name: " + name + "  exists in  group " + groupName,
				"ResourceGroup::insertFileLocation");
		}
	}

	template<> ResourceGroupManager* Singleton<ResourceGroupManager>::ms_Singleton = 0;

	ResourceGroupManager* ResourceGroupManager::getSingltonPtr()
	{
		return ms_Singleton;
	}
	ResourceGroupManager& ResourceGroupManager::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	ResourceGroupManager::ResourceGroupManager()
	{

	}
	//-------------------------------------------------------------//
	ResourceGroupManager::~ResourceGroupManager()
	{

	}
	//-------------------------------------------------------------//
	FileSystem* ResourceGroupManager::addResourceLocation(const String& name, const String& type, const String& group, bool recursive)
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
		StringVectorPtr pStringVector =  fileSystem->find("*", recursive);
		StringVector::iterator sit = pStringVector->begin(), sitend = pStringVector->end();
		while (sit != sitend)
		{
			gp->insertFileLocation(*sit, fileSystem);
			++sit;
		}

		return fileSystem;
	}
	//-------------------------------------------------------------//
	ResourceGroupManager::ResourceGroup* ResourceGroupManager::getResourceGroup(const String& group)
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
				"ResourceGroupManager::getResourceGroup");
		}

		return gp;

	}
	//-------------------------------------------------------------//
	ResourceGroupManager::ResourceGroup* ResourceGroupManager::createResourceGroup(const String& group)
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
				"ResourceGroupManager::createResourceGroup");
		}
		return gp;
	}
	void ResourceGroupManager::initResourceGroup(const String& group)
	{
		ResourceGroupMap::iterator it = mResourceGroupMap.find(group);
		if(it != mResourceGroupMap.end() &&
			it->second->state == ResourceGroup::RGS_UNINITIALISED)
		{
			it->second->state = ResourceGroup::RGS_INITIALISING;
			//if we need to implement script
			//add content here
			it->second->state = ResourceGroup::RGS_INITIALISED;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"We does not have a resource group called : " + group,
				"ResourceGroupManager::initResourceGroup");
		}
	}
	//-------------------------------------------------------------//
	void ResourceGroupManager::initAllResourceGroup()
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
	//-------------------------------------------------------------//
	void ResourceGroupManager::destroyResourceGroup(const String& group)
	{

	}
	//-------------------------------------------------------------//
	void ResourceGroupManager::destroyAllResourceGroup()
	{

	}
	//-------------------------------------------------------------//
	void ResourceGroupManager::loadResourceGroup(const String& group)
	{

	}
	//-------------------------------------------------------------//
	void ResourceGroupManager::unloadResourceGroup(const String& group)
	{

	}

}