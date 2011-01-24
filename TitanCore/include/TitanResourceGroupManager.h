#ifndef _TITAN_RESOURCEGROUPMANAGER__HH
#define _TITAN_RESOURCEGROUPMANAGER__HH

#include "TitanPrerequisites.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport ResourceGroupManager: public Singleton<ResourceGroupManager>, public GeneralAlloc
	{
	public:
		struct ResourceGroup
		{
			enum Status
			{
				RGS_UNLOADED = 0,
				RGS_LOADING	 = 1,
				RGS_LOADED	 = 2
			};
			String	name;
			Status	state;
			typedef list<Resource*>::type ResourceList;
		};
		typedef map<String, ResourceGroup*>::type ResourceGroupMap;
	public:
		ResourceGroupManager();

		virtual ~ResourceGroupManager();

		FileSystem*	addResourceLocation(const String& name, const String& type, const String& group, bool recursive = false);

		static ResourceGroupManager*	getSingltonPtr();

		static ResourceGroupManager&	getSingleton();

	protected:
	};
}
#endif