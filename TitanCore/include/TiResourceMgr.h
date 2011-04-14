#ifndef _TITAN_RESOURCEMANAGER_HH
#define _TITAN_RESOURCEMANAGER_HH

#include "TitanPrerequisites.h"
#include "TitanResource.h"
#include "TitanCommon.h"
#include "TiResourceGroupMgr.h"

namespace Titan
{

	class _DllExport ResourceMgr: public GeneralAlloc
	{
	public:
		typedef hash_map<String, ResourcePtr>::type ResourceMap;
		typedef map<ResourceHandle, ResourcePtr>::type ResourceHandleMap;
		typedef hash_map<String, ResourceMap>::type ResourceGroupMap;

		//true means it is a new resouce we just created
		typedef std::pair<bool, ResourcePtr>	CreatedResource;

	public:
		ResourceMgr();

		virtual ~ResourceMgr() = 0;

		CreatedResource create(const String& name, const String& group = ResourceGroupMgr::GENERAL_RESOURCE_GROUP, AnyMap* extraParams = 0);

		void			remove(const String& name);

		void			remove(ResourceHandle ResourceID);

		void			removeAll();

		void			removeUnreferencedResources();

		ResourcePtr		load(const String& name, const String& group = ResourceGroupMgr::GENERAL_RESOURCE_GROUP, AnyMap* extraParams = 0);

		void			unload(const String& name);

		void			unload(ResourceHandle ResourceID);

		void			unloadAll();

		//no use now
		void			unloadUnreferencedResources();

		int				getLoadOrder() const { return mLoadOrder; }

		ResourceHandle	getNextHandle(){ return ++mNextHandle; }

		const String&	getType()const { return mType; }
		
		virtual ResourcePtr getByName(const String& name, const String& groupName = ResourceGroupMgr::GENERAL_RESOURCE_GROUP);
	protected:

		virtual Resource*	createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams) = 0;
		
		virtual	void		addImpl(ResourcePtr& res);


	protected:
		int						mLoadOrder;
		String					mType;
		ResourceMap				mResourceMap;
		ResourceHandleMap		mResourceHandleMap;
		ResourceHandle			mNextHandle;
	};

}

#endif