#ifndef _TITAN_RESOURCEGROUPMANAGER__HH
#define _TITAN_RESOURCEGROUPMANAGER__HH

#include "TiPrerequisites.h"
#include "TiSingleton.h"
#include "TiResource.h"
#include "TiDataStream.h"

namespace Titan
{
	class _DllExport ResourceGroupMgr: public Singleton<ResourceGroupMgr>, public GeneralAlloc
	{
	public:
		typedef list<ResourcePtr>::type ResourceList;
		typedef map<int, ResourceList*>::type ResourceOrderListMap;
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
		typedef map<String, LocationInfo*>::type	FileLocationMap;
		//use to collect found scripts
		typedef SharedPtr<FileLocationMap> FileLocationMapPtr;
		typedef map<ScriptLoader*, FileLocationMapPtr>::type ScriptLoaderToFileLocationMap;

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

			FileLocationMap fileLocationMap;

			ResourceOrderListMap	resourceOrderListMap;

			void insertFileLocation(const String& name, const String& relativePath, FileSystem* fileSystem);
		};

		typedef map<String, ResourceGroup*>::type ResourceGroupMap;

		typedef map<int, ScriptLoader*>::type ScriptLoaderMap;

		static String	GENERAL_RESOURCE_GROUP;
		static String	INTERNAL_RESOURCE_GROUP;
	public:
		ResourceGroupMgr();

		virtual ~ResourceGroupMgr();

		//for first editon of Titan, we should write all assets paths into config file
		FileSystem*	addResourceLocation(const String& name, const String& type, const String& group, bool recursive = false);
		//test that whether we made it right
		ResourceGroup*	getResourceGroup(const String& group);

		ResourceGroup*	createResourceGroup(const String& group);

		//no use now
		void		initResourceGroup(const String& group);
		//no use now
		void		initAllResourceGroup();

		void		destroyResourceGroup(const String& group);

		void		destroyAllResourceGroup();

		void		loadResourceGroup(const String& group);

		void		unloadResourceGroup(const String& group);

		void		addCreatedResource(const String& group, ResourcePtr& res);
	
		void		parseResourceGroup(ResourceGroup* rg);

		DataStreamPtr	openResource(const String& name, const String& group = GENERAL_RESOURCE_GROUP);

		void _registerScriptLoader(ScriptLoader* su);

        void _unregisterScriptLoader(ScriptLoader* su);


		static ResourceGroupMgr*	getSingltonPtr();

		static ResourceGroupMgr&	getSingleton();

	protected:
		//to boost the speed
		ResourceGroup*			mCurrentGroup;
		ResourceGroupMap		mResourceGroupMap;

		ScriptLoaderMap			mScriptLoaderMap;
	};
}
#endif