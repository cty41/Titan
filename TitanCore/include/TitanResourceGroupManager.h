#ifndef _TITAN_RESOURCEGROUPMANAGER__HH
#define _TITAN_RESOURCEGROUPMANAGER__HH

#include "TitanPrerequisites.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport ResourceGroupManager: public Singleton<ResourceGroupManager>, public GeneralAlloc
	{
	public:
		struct ResourceGroup : public GeneralAlloc
		{
			enum Status
			{
				RGS_UNINITIALISED	= 0,
				RGS_INITIALISING	= 1,
				RGS_INITIALISED		= 2,
				RGS_LOADING			= 3,
				RGS_LOADED			= 4
			};
			String	groupName;
			Status	state;
			typedef map<String, FileSystem*>::type	FileLocationMap;
			FileLocationMap fileLocationMap;

			void insertFileLocation(const String& name, FileSystem* fileSystem);
		};

		typedef map<String, ResourceGroup*>::type ResourceGroupMap;
	public:
		ResourceGroupManager();

		virtual ~ResourceGroupManager();

		FileSystem*	addResourceLocation(const String& name, const String& type, const String& group, bool recursive = false);

		static ResourceGroupManager*	getSingltonPtr();

		static ResourceGroupManager&	getSingleton();

	protected:

		ResourceGroupMap		mResourceGroupMap;
	};
}
#endif