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
		StringVectorPtr namesPtr(TITAN_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);
		StringVectorPtr pathsPtr(TITAN_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);	
		
		searchDir(mName + wildcard, recursive,namesPtr, pathsPtr);

		return namesPtr;
	}
	//-------------------------------------------------------------//
	void CommonFileSystem::find(const String& wildcard, bool recursive, StringVectorPtr *nameArray, StringVectorPtr* relPathArray)
	{
		StringVectorPtr namesPtr(TITAN_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);
		StringVectorPtr pathsPtr(TITAN_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);
		*nameArray = namesPtr, *relPathArray = pathsPtr;

		searchDir(wildcard, recursive,namesPtr, pathsPtr);
	}
	//-------------------------------------------------------------//
	void CommonFileSystem::searchDir(const String& pattern, bool recursive, StringVectorPtr stringVector, StringVectorPtr pathVector)
	{
		_finddata_t findData;
		String fullpath = mName, fullpathName;
		if(fullpath.at(fullpath.length() - 1) != '/' && fullpath.at(fullpath.length() - 1) != '\\' )
			fullpath += '/';

		size_t slashPos = pattern.find_last_of('/');
		if(slashPos != String::npos)
		{
			fullpath += pattern.substr(0, slashPos + 1);
			fullpathName = fullpath + pattern.substr(slashPos + 1, pattern.length() - 1);
		}
		else
			fullpathName = fullpath + pattern;
		
		intptr_t handle = _findfirst(fullpathName.c_str(), &findData);
		int ret = 0;
		StringVector dirVector;
		//woo,there has files!
		while (handle != -1 && ret != -1)
		{
			//only add file not folders
			if((findData.attrib & _A_SUBDIR) == 0)
			{
				stringVector->push_back(findData.name);
				pathVector->push_back(fullpath);
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
			StringVector::iterator it = dirVector.begin(), itend = dirVector.end();
			while(it != itend)
			{
				size_t slashpos = pattern.find_last_of('/');
				String subDir;
				if(slashpos != String::npos)
					subDir = pattern.substr(0, slashpos + 1) +  *it + pattern.substr(slashpos, pattern.length() -1);
				else
					subDir = *it + '/' + pattern;
				searchDir(subDir, recursive, stringVector, pathVector);
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