#include "TitanStableHeader.h"
#include "CommonFileSystem.h"
#include <io.h>


namespace Titan
{
	CommonFileSystem::CommonFileSystem(const String& name, const String& type)
		:FileSystem(name, type)
	{
	}
	//-------------------------------------------------------------//
	CommonFileSystem::~CommonFileSystem()
	{}
	//-------------------------------------------------------------//
	void CommonFileSystem::load()
	{
		//for test
		String testTmp(mName);
	}
	//-------------------------------------------------------------//
	StringVectorPtr CommonFileSystem::find(const String& wildcard, bool recursive)
	{
		StringVectorPtr stringPtr(TITAN_NEW_T(StringVector, MEMCATEGORY_GENERAL)());

		String pathName(mName + wildcard);

		_finddata_t findData;
		intptr_t handle = _findfirst(pathName.c_str(), &findData);
		int ret = 0;
		//there has file!
		while (handle != -1 && ret != -1)
		{
			//only add file not floders
			if((findData.attrib & _A_SUBDIR) == 0)
			{
				stringPtr->push_back(mName + findData.name);
			}
			ret = _findnext(handle, &findData);
		}

		return stringPtr;
	}
	//-------------------------------------------------------------//





	FileSystem* CommonFileSystemFactory::createInstance(const String& name, const String& type)
	{
		return TITAN_NEW CommonFileSystem(name, type);
	}
	//-------------------------------------------------------------//
	void CommonFileSystemFactory::destroyInstance(FileSystem* file)
	{
		TITAN_DELETE file;
	}
	
}