#ifndef __TITAN_CONFIGFILE__HH
#define __TITAN_CONFIGFILE__HH

#include "TiPrerequisites.h"
#include "TiIteratorWrapper.h"

namespace Titan
{

	class _DllExport ConfigFile : public GeneralAllocatedObject
	{
	public:
		typedef multimap<String, String>::type PropertyMap;
		typedef MapIterator<PropertyMap>	PropertyMapIterator;
		typedef	map<String, PropertyMap*>::type SectionMap;
		typedef MapIterator<SectionMap>		SectionMapIterator;
	public:
		ConfigFile();

		~ConfigFile();

		void	load(const String& name, const String& separators = "\t:=");

		SectionMapIterator	getSectionMapIterator();

		PropertyMapIterator	getPropertyMapIterator(const String& sectionName);

	protected:
		SectionMap	mSectionMap;

	};
}
#endif