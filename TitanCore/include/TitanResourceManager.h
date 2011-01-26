#ifndef _TITAN_RESOURCEMANAGER_HH
#define _TITAN_RESOURCEMANAGER_HH

#include "TitanPrerequisites.h"
#include "TitanResource.h"
#include "TitanCommon.h"

namespace Titan
{

	class _DllExport ResourceMgr: public GeneralAlloc
	{
	public:
		typedef hash_map<String, ResourcePtr>::type ResourceMap;
		typedef map<ResourceHandle, ResourcePtr>::type ResourceHandleMap;
		typedef hash_map<String, ResourceMap>::type ResourceGroupMap;

	public:
		ResourceMgr();

		~ResourceMgr();

		ResourcePtr create(const String& name, const String& group, AnyMap* extraParams = 0);

		void	remove(const String& name);

		void	remove(ResourceHandle ResourceID);

		void	removeAll();

		void	removeUnreferencedResources();

		void	load(const String& name);

		void	load(ResourceHandle ResourceID);

		void	unload(const String& name);

		void	unload(ResourceHandle ResourceID);

		void	unloadAll();

		void	unloadUnreferencedResources();

		ResourceHandle	getNextHandle(){ return ++mNextHandle; }
	protected:

		virtual Resource* createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams = 0) = 0;

		virtual	void		addImpl(ResourcePtr* res) = 0;

	protected:
		String			mType;
		ResourceMap		mResourceMap;
		ResourceHandleMap	mResourceHandleMap;
		ResourceHandle	mNextHandle;
	};

}

#endif