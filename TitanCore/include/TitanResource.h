#ifndef _TITAN_RESOURCE_HH
#define _TITAN_RESOURCE_HH

#include "TitanPrerequisites.h"
#include "TitanSharedPtr.h"
#include "TitanDataStream.h"


namespace Titan
{
	//unique id for every resource, is this type right for gcc?
	typedef unsigned long long int ResourceHandle;

	enum LoadState
	{
		LS_UNPREPARED	= 0,
		LS_PREPARING	= 1,
		LS_PREPARED		= 2,
		LS_LOADING		= 3,
		LS_LOADED		= 4
	};

	class _DllExport Resource: public GeneralAlloc
	{
	public:
		Resource(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group);

		virtual ~Resource();

		virtual void	load();

		virtual	void	unload();

		virtual void	prepare();

		virtual void	unprapare();

		bool	isPrepared() const { return (mLoadState >= LS_PREPARED);}

		bool	isLoaded() const { return (mLoadState == LS_LOADED); }

		void	changeGroup(const String& newGroup);

		const String&	getName() const { return mName; }

		const String&	getGroup() const { return mGroup; }

		ResourceHandle	getID() const { return mID; }

		ResourceMgr*	getCreator() const { return mMgr; }


	protected:
		virtual void	prepareImpl();

		virtual void	unprepareImpl();

		virtual void	loadImpl() = 0;

		virtual	void	unloadImpl() = 0;


	protected:
		ResourceMgr*			mMgr;
		String					mName;
		String					mGroup;
		ResourceHandle			mID;
		LoadState				mLoadState;
		MemoryDataStreamPtr		mPreparedData;

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