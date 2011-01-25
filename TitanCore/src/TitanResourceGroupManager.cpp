#include "TitanStableHeader.h"
#include "TitanResourceGroupManager.h"
#include "FileSystemManager.h"

namespace Titan
{
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
}