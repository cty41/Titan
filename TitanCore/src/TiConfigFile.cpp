#include "TitanStableHeader.h"
#include "TiConfigFile.h"
#include "TiException.h"
#include "TiStringFuncs.h"

namespace Titan
{
	ConfigFile::ConfigFile()
	{

	}
	//-------------------------------------------------------------------------------//
	ConfigFile::~ConfigFile()
	{
		SectionMap::iterator it = mSectionMap.begin(), itend = mSectionMap.end();
		while(it != itend)
		{
			(it->second)->clear();
			TITAN_DELETE_T(it->second, PropertyMap, MEMCATEGORY_GENERAL);
			++it;
		}
		mSectionMap.clear();
	}
	//-------------------------------------------------------------------------------//
	void ConfigFile::load(const String& name, const String& separators)
	{
		std::ifstream fp;

		fp.open(name.c_str(), std::ios::in | std::ios::binary );

		if(fp == 0)
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				" Can not find config file called: "+ name, 
				"ConfigFile::load");
			return ;
		}

		DataStreamPtr stream(TITAN_NEW FileStreamDataStream(name, &fp, false)); 

		String line, optName, optVal;
		PropertyMap* currentMap = 0;
		while(!stream->eof())
		{
			line = stream->getLine();
			if(line.length() > 0 && line.at(0) != '#')
			{
				if(line.at(0) == '[' && line.at(line.length() - 1) == ']')
				{
					String sectionName = line.substr(1, line.length() - 2);
					SectionMap::iterator it = mSectionMap.find(sectionName);
					if(it == mSectionMap.end())
					{
						currentMap = TITAN_NEW_T(PropertyMap, MEMCATEGORY_GENERAL)();
						mSectionMap.insert(SectionMap::value_type(sectionName, currentMap));
					}
					else
						currentMap = it->second;
				}
				else
				{
					String::size_type separator_pos = line.find_first_of(separators, 0);
					if (separator_pos != String::npos)
					{
						optName = line.substr(0, separator_pos);
						/* Find the first non-seperator character following the name */
						String::size_type nonseparator_pos = line.find_first_not_of(separators, separator_pos);
						/* ... and extract the value */
						/* Make sure we don't crash on an empty setting (it might be a valid value) */
						optVal = (nonseparator_pos == String::npos) ? "" : line.substr(nonseparator_pos);

						StringUtil::trim(optVal);
						StringUtil::trim(optName);

						currentMap->insert(PropertyMap::value_type(optName, optVal));
					}
				}
			}
		}
		fp.close();
	}
	//-------------------------------------------------------------------------------//
	ConfigFile::SectionMapIterator	ConfigFile::getSectionMapIterator()
	{
		return SectionMapIterator(mSectionMap.begin(), mSectionMap.end());
	}
	//-------------------------------------------------------------------------------//
	ConfigFile::PropertyMapIterator ConfigFile::getPropertyMapIterator(const String& sectionName)
	{
		SectionMap::iterator it = mSectionMap.find(sectionName);
		return PropertyMapIterator(it->second->begin(), it->second->end());
	}
}