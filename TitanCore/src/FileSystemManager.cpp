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
	//-------------------------------------------------------------------------------//

	FileSystemManager::FileSystemManager()
	{

	}
	//-------------------------------------------------------------------------------//
	FileSystemManager::~FileSystemManager()
	{
		unloadAllInstances();
		removeAllFactory();
	}	
	//-------------------------------------------------------------------------------//
	FileSystem* FileSystemManager::load(const String& name, const String& type)
	{
		FileSystem* pFile = 0;

		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.find(type);
		if(it != mFileSystemFactoryMap.end())
		{
			FileSystemMap::iterator fit = mFileSystemMap.find(name);
			if(fit == mFileSystemMap.end())
			{
				if(name.at(name.length() - 1) != '/')
					pFile = it->second->createInstance(name + '/', type);
				else
					pFile = it->second->createInstance(name, type);
				pFile->load();
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
	//-------------------------------------------------------------------------------//
	void FileSystemManager::unload(const String& name)
	{
		FileSystemMap::iterator it = mFileSystemMap.find(name);
		if(it != mFileSystemMap.end())
		{
			TITAN_DELETE it->second;
			mFileSystemMap.erase(it);
		}	
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The FileSystem with this name " + name + " does exist!",
				"FileSystemManager::load");
		}
	}
	//-------------------------------------------------------------------------------//
	void FileSystemManager::unload(FileSystem* system)
	{
		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.find(system->getType());
		if(it != mFileSystemFactoryMap.end())
		{
			TITAN_DELETE it->second;
			mFileSystemFactoryMap.erase(it++);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The FileSystemFactory with this type " + system->getType() + " does not exist!",
				"FileSystemManager::load");
		}
	}
	//-------------------------------------------------------------------------------//
	void FileSystemManager::unloadAllInstances()
	{
		FileSystemMap::iterator it = mFileSystemMap.begin(), itend = mFileSystemMap.end();
		while(it != itend)
		{
			TITAN_DELETE it->second;
			mFileSystemMap.erase(it++);
		}
	}		
	//-------------------------------------------------------------------------------//
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
	//-------------------------------------------------------------------------------//
	void FileSystemManager::removeFileSystemFactory(FileSystemFactory* factory)
	{
		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.find(factory->getType());
		if(it != mFileSystemFactoryMap.end())
		{
			mFileSystemFactoryMap.erase(it);
			TITAN_DELETE factory;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The FileSystemFactory with this type " + factory->getType() + " does not exist!",
				"FileSystemManager::removeFileSystemFactory");
		}
	}
	//-------------------------------------------------------------------------------//
	void FileSystemManager::removeAllFactory()
	{
		FileSystemFactoryMap::iterator it = mFileSystemFactoryMap.begin(), itend = mFileSystemFactoryMap.end();
		while(it != itend)
		{
			TITAN_DELETE it->second;
			mFileSystemFactoryMap.erase(it++);
		}
	}
	//-------------------------------------------------------------------------------//
	FileSystemMapIterator FileSystemManager::getFileSystemMapIterator()
	{
		return FileSystemMapIterator(mFileSystemMap.begin(), mFileSystemMap.end());
	}

}