#ifndef _TITAN_COMMONFILESYSTEM_HH
#define _TITAN_COMMONFILESYSTEM_HH

#include "TitanPrerequisites.h"
#include "FileSystem.h"

namespace Titan
{
	class _DllExport CommonFileSystem : public FileSystem
	{
	public:
		CommonFileSystem(const String& name, const String& type);

		~CommonFileSystem();

		void	load();

		void	unload(){};

		StringVectorPtr find(const String& wildcard, bool recursive);
	protected:
		void	searchDir(const String& subdir, StringVectorPtr stringVector, bool recursive);
	};


	class _DllExport CommonFileSystemFactory : public FileSystemFactory
	{
	public:
		CommonFileSystemFactory(): FileSystemFactory("CommonFileSystem")
		{}

		~CommonFileSystemFactory(){}; 

		FileSystem*		createInstance(const String& name, const String& type);

		void			destroyInstance(FileSystem* file) ;
	};
}
#endif