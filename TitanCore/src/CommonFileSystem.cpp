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
		StringVectorPtr stringPtr(TITAN_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);
		
		searchDir(mName + wildcard, stringPtr, recursive);

		return stringPtr;
	}
	//-------------------------------------------------------------//
	void CommonFileSystem::searchDir(const String& subdir, StringVectorPtr stringVector, bool recursive)
	{
		_finddata_t findData;
		intptr_t handle = _findfirst(subdir.c_str(), &findData);
		int ret = 0;
		StringVector dirVector;
		//woo,there has files!
		while (handle != -1 && ret != -1)
		{
			//only add file not folders
			if((findData.attrib & _A_SUBDIR) == 0)
			{
				stringVector->push_back(findData.name);
			}
			else
			{
				if(recursive &&findData.name[0] != '.')
				{
					dirVector.push_back(String(findData.name));
				}
			}

			ret = _findnext(handle, &findData);
		}
		if(handle != -1)
			_findclose(handle);

		if(recursive)
		{
			String baseDir = subdir;
			if(baseDir.at(baseDir.length()-1) == '*')
				baseDir.erase(baseDir.length()-1);
			StringVector::iterator it = dirVector.begin(), itend = dirVector.end();
			while(it != itend)
			{
				searchDir(baseDir + *it + "/*", stringVector, recursive);
				++it;
			}
		}
	}





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