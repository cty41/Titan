#ifndef _TITAN_FILESYSTEMMANAGER_HH
#define _TITAN_FILESYSTEMMANAGER_HH

#include "TiPrerequisites.h"
#include "TiFileSystem.h"
#include "TiSingleton.h"
#include "TiIteratorWrapper.h"

namespace Titan
{
	typedef map<String, FileSystemFactory*>::type FileSystemFactoryMap;
	typedef map<String, FileSystem*>::type FileSystemMap;
	typedef MapIterator<FileSystemMap> FileSystemMapIterator;
	class _DllExport FileSystemManager : public Singleton<FileSystemManager>, public GeneralAlloc
	{
	public:
		FileSystemManager();

		~FileSystemManager();

		FileSystem*		load(const String& name, const String& type);

		void			unload(const String& name);

		void			unload(FileSystem* system);

		void			unloadAllInstances();

		void			addFileSystemFactory(FileSystemFactory* factory);

		void			removeFileSystemFactory(FileSystemFactory* factory);

		void			removeAllFactory();

		FileSystemMapIterator	getFileSystemMapIterator();

		static FileSystemManager* getSingletonPtr();

		static FileSystemManager& getSingleton();


	
	protected:
		FileSystemMap	mFileSystemMap;
		FileSystemFactoryMap	mFileSystemFactoryMap;
	};
}
#endif