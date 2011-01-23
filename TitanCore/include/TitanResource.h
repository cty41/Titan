#ifndef _TITAN_RESOURCE_HH
#define _TITAN_RESOURCE_HH

#include "TitanPrerequisites.h"
#include "TitanSharedPtr.h"


namespace Titan
{
	//unique id for every resource, is this type right for gcc?
	typedef unsigned long long int ResourceHandle;

	enum LoadState
	{
		LS_UNLOAD	= 0,
		LS_LOADING	= 1,
		LS_LOADED	= 2
	};

	class _DllExport Resource: public GeneralAlloc
	{
	public:
		Resource(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group);

		virtual ~Resource();

		void	load();

		void	unload();

		void	changeGroup(const String& newGroup);

		const String&	getName() const { return mName; }

		const String&	getGroup() const { return mGroup; }

		ResourceHandle	getID() const { return mID; }

	protected:
		ResourceMgr*		mMgr;
		String				mName;
		String				mGroup;
		ResourceHandle		mID;
		LoadState			mLoadState;

	};

	class _DllExport ResourcePtr : public SharedPtr<Resource>
	{
	public:
		ResourcePtr()
			:SharedPtr<Resource>()
		{
		}

		explicit ResourcePtr(Resource* res)
			: SharedPtr<Resource>(res)
		{
		}
	};
}

#endif