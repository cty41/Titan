#include "TitanStableHeader.h"
#include "TitanResource.h"
#include "TiResourceGroupMgr.h"
#include "TiResourceMgr.h"
#include "TitanDataStream.h"

namespace Titan
{
	Resource::Resource(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
	: mMgr(mgr), mName(name), mID(id), mGroup(group), mLoadState(LS_UNPREPARED)
	{
		
	}
	//-------------------------------------------------------------------------------//
	Resource::~Resource()
	{

	}
	//-------------------------------------------------------------------------------//
	void Resource::prepareImpl()
	{
		DataStreamPtr dataStream = ResourceGroupMgr::getSingltonPtr()->openResource(mName, mGroup);
		mPreparedData = MemoryDataStreamPtr(TITAN_NEW MemoryDataStream(dataStream));
	}
	//-------------------------------------------------------------------------------//
	void Resource::unprepareImpl()
	{
		mPreparedData.setNull();
	}
	//-------------------------------------------------------------------------------//
	void Resource::prepare()
	{
		if(!isPrepared())
		{
			mLoadState = LS_PREPARING;
			prepareImpl();
			mLoadState = LS_PREPARED;
		}
	}
	//-------------------------------------------------------------------------------//
	void Resource::unprapare()
	{
		if(isPrepared())
		{
			unprepareImpl();
			mLoadState = LS_UNPREPARED;
		}
	}
	//-------------------------------------------------------------------------------//
	void Resource::load()
	{
		prepare();

		if(!isLoaded())
		{
			mLoadState = LS_LOADING;
			loadImpl();
			mLoadState = LS_LOADED;
		}	
	}
	//-------------------------------------------------------------------------------//
	void Resource::unload()
	{
		if(isLoaded())
		{
			unloadImpl();
			mLoadState = LS_PREPARED;
		}
	}
	//-------------------------------------------------------------------------------//
	void Resource::changeGroup(const String& newGroup)
	{
	}
}