#ifndef _TITAN_RESOURCEGROUPMANAGER__HH
#define _TITAN_RESOURCEGROUPMANAGER__HH

#include "TitanPrerequisites.h"
#include "Singleton.h"
#include "TitanResource.h"

namespace Titan
{
	class _DllExport ResourceGroupManager: public Singleton<ResourceGroupManager>, public GeneralAlloc
	{
	public:
		struct ResourceGroup : public GeneralAlloc
		{
			struct LocationInfo : public GeneralAlloc
			{
				String			relativePath;
				FileSystem*		pFileSystem;

				LocationInfo(const String& relPath, FileSystem*	fileSystem)
				{
					relativePath = relPath;
					pFileSystem = fileSystem;
				}
			};
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
			typedef map<String, LocationInfo>::type	FileLocationMap;
			FileLocationMap fileLocationMap;
			typedef list<ResourcePtr>::type ResourceList;
			typedef map<int, ResourceList*>::type ResourceOrderListMap;
			ResourceOrderListMap	resourceOrderListMap;

			void insertFileLocation(const String& name, const String& relativePath, FileSystem* fileSystem);
		};

		typedef map<String, ResourceGroup*>::type ResourceGroupMap;

		static String	GENERAL_RESOURCE_GROUP;
	public:
		ResourceGroupManager();

		virtual ~ResourceGroupManager();

		//for first editon of Titan, we should write all assets paths into config file
		FileSystem*	addResourceLocation(const String& name, const String& type, const String& group, bool recursive = false);
		//test that whether we made it right
		ResourceGroup*	getResourceGroup(const String& group);

		ResourceGroup*	createResourceGroup(const String& group);

		//no use now
		void initResourceGroup(const String& group);
		//no use now
		void initAllResourceGroup();

		void destroyResourceGroup(const String& group);

		void destroyAllResourceGroup();

		void loadResourceGroup(const String& group);

		void unloadResourceGroup(const String& group);

		void addCreatedResource(const String& group, Resource& res);


		static ResourceGroupManager*	getSingltonPtr();

		static ResourceGroupManager&	getSingleton();

	protected:
		//to boost the speed
		ResourceGroup*			mCurrentGroup;
		ResourceGroupMap		mResourceGroupMap;
	};
}
#endif