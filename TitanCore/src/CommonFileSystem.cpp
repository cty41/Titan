#include "TitanStableHeader.h"
#include "CommonFileSystem.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>


namespace Titan
{
	static bool is_absolute_path(const char* path)
	{
		if (isalpha(uchar(path[0])) && path[1] == ':')
			return true;
	}

	static String concatenate_path(const String& base, const String& name)
	{
		if (base.empty() || is_absolute_path(name.c_str()))
			return name;
		else
			return base + '/' + name;
	}

	CommonFileSystem::CommonFileSystem(const String& name, const String& type)
		:FileSystem(name, type)
	{
	}
	//-------------------------------------------------------------//
	CommonFileSystem::~CommonFileSystem()
	{

	}
	//-------------------------------------------------------------//
	void CommonFileSystem::load()
	{
		//for test
		String testTmp(mName);
	}
	//-------------------------------------------------------------//
	DataStreamPtr CommonFileSystem::open(const String& filename)
	{
		String full_path = concatenate_path(mName, filename);

		// Use filesystem to determine size 
		// (quicker than streaming to the end and back)
		struct stat tagStat;
		int ret = stat(full_path.c_str(), &tagStat);
		assert(ret == 0 && "Problem getting file size" );
		(void)ret;  // Silence warning

		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::in | std::ios::binary;
		std::istream* baseStream = 0;
		std::ifstream* roStream = 0;
		std::fstream* rwStream = 0;


		mode |= std::ios::out;
		rwStream = TITAN_NEW_T(std::fstream, MEMCATEGORY_GENERAL)();
		rwStream->open(full_path.c_str(), mode);

		// Should check ensure open succeeded, in case fail for some reason.
		if (baseStream->fail())
		{
			TITAN_DELETE_T(roStream, basic_ifstream, MEMCATEGORY_GENERAL);
			TITAN_DELETE_T(rwStream, basic_fstream, MEMCATEGORY_GENERAL);
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"Cannot open file: " + filename,
				"CommonFileSystem::open");
		}

		/// Construct return stream, tell it to delete on destroy
		FileStreamDataStream* stream = 0;
		if (rwStream)
		{
			// use the writeable stream 
			stream = TITAN_NEW FileStreamDataStream(filename,
				rwStream, tagStat.st_size, true);
		}
		else
		{
			// read-only stream
			stream = TITAN_NEW FileStreamDataStream(filename,
				roStream, tagStat.st_size, true);
		}
		return DataStreamPtr(stream);
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