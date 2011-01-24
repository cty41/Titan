#include "TitanStableHeader.h"
#include "TitanResourceGroupManager.h"
#include "FileSystemManager.h"

namespace Titan
{
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
		FileSystem* fileSystem = FileSystemManager::getSingletonPtr()->load(name, type);
		return fileSystem;
	}
}