#include "TitanStableHeader.h"
#include "FileSystemManager.h"

namespace Titan
{
	template<> FileSystemManager* Singleton<FileSystemManager>::ms_Singleton = 0;

	FileSystemManager* FileSystemManager::getSingletonPtr()
	{
		return ms_Singleton;
	}
	FileSystemManager& FileSystemManager::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	//-------------------------------------------------------------//

	FileSystemManager::FileSystemManager()
	{

	}
	//-------------------------------------------------------------//
	FileSystemManager::~FileSystemManager()
	{

	}
	//-------------------------------------------------------------//
	FileSystem* FileSystemManager::load(const String& name, const String& type)
	{
		FileSystem* pFile = 0;

		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.find(type);
		if(it != mFileSystemFactoryMap.end())
		{
			FileSystemMap::iterator fit = mFileSystemMap.find(name);
			if(fit == mFileSystemMap.end())
			{
				pFile = it->second->createInstance(name, type);
				mFileSystemMap.insert(FileSystemMap::value_type(name, pFile));
				
			}
			else
			{
				pFile = fit->second;
			}
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The FileSystemFactory with this type " + type + " does not exist!",
				"FileSystemManager::load");
		}
		return pFile;
	}
	//-------------------------------------------------------------//
	void FileSystemManager::unload(const String& name)
	{
		FileSystemMap::iterator it = mFileSystemMap.find(name);
		if(it != mFileSystemMap.end())
		{
			FileSystemFactoryMap::iterator fit = mFileSystemFactoryMap.find(it->second->getType());
			if(fit != mFileSystemFactoryMap.end())
			{
				fit->second->destroyInstance(it->second);
			}
		}
		else
		{
			{
				TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
					"The FileSystem with this name " + name + " does exist!",
					"FileSystemManager::load");
			}
		}
	}
	//-------------------------------------------------------------//
	void FileSystemManager::unload(FileSystem* system)
	{
		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.find(system->getType());
		if(it != mFileSystemFactoryMap.end())
		{
			it->second->destroyInstance(system);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The FileSystemFactory with this type " + system->getType() + " does not exist!",
				"FileSystemManager::load");
		}
	}
	//-------------------------------------------------------------//
	void FileSystemManager::addFileSystemFactory(FileSystemFactory* factory)
	{
		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.find(factory->getType());
		if(it == mFileSystemFactoryMap.end())
		{
			mFileSystemFactoryMap.insert(FileSystemFactoryMap::value_type(factory->getType(), factory));
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The FileSystemFactory with this type " + factory->getType() + " does exist!",
				"FileSystemManager::addFileSystemFactory");
		}
	}
}