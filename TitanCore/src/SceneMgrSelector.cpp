#include "TitanStableHeader.h"
#include "SceneMgrSelector.h"


namespace Titan
{
	template<> SceneMgrSelector* Singleton<SceneMgrSelector>::ms_Singleton = 0;
	SceneMgrSelector* SceneMgrSelector::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
	SceneMgrSelector& SceneMgrSelector::getSingleton(void)
	{  
		assert( ms_Singleton );  return ( *ms_Singleton );  
	}
	//-------------------------------------------------------------//
	SceneMgrSelector::SceneMgrSelector()
	{
		addFactory(TITAN_NEW SceneMgrFactory());
	}
	//-------------------------------------------------------------//
	SceneMgrSelector::~SceneMgrSelector()
	{
		SceneMgrMap::iterator i = mSceneMgrMap.begin(), iend = mSceneMgrMap.end();
		while(i != iend)
		{
			TITAN_DELETE (i->second);
			++i;
		}
		mSceneMgrMap.clear();

		FactoryMap::iterator it = mFactoryMap.begin(), itend = mFactoryMap.end();
		while (it != itend)
		{
			TITAN_DELETE(it->second);
			++it;
		}
		mFactoryMap.clear();
	}
	//-------------------------------------------------------------//
	void SceneMgrSelector::addFactory(SceneMgrFactory* factory)
	{
		FactoryMap::iterator it = mFactoryMap.find(factory->getType());
		if(it != mFactoryMap.end())
		{
			TITAN_DELETE factory;
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"this type of factory has existed!!",
				"SceneMgrSelector::addFactory");
		}
		else
		{
			mFactoryMap.insert(FactoryMap::value_type(factory->getType(), factory));
		}
	}
	//-------------------------------------------------------------//
	void SceneMgrSelector::removeFactory(SceneMgrFactory* factory)
	{
		FactoryMap::iterator it = mFactoryMap.find(factory->getType());
		if(it != mFactoryMap.end())
		{
			TITAN_DELETE it->second;
			mFactoryMap.erase(it);

		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"this type of factory did not existed!!",
				"SceneMgrSelector::removeFactory");
		}
	}
	//-------------------------------------------------------------//
	SceneMgr*	SceneMgrSelector::createSceneMgr(const String& name, SceneMgrType smt)
	{
		SceneMgrMap::iterator i = mSceneMgrMap.find(smt);
		if(i != mSceneMgrMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"this type of SceneMgr has existed!!",
				"SceneMgrSelector::createSceneMgr");
			return 0;
		}
		else
		{
			SceneMgr* mgr = 0;
			FactoryMap::iterator it = mFactoryMap.find(smt);
			if(it != mFactoryMap.end())
			{
				mgr = it->second->createSceneMgr(name);
			}
			else
			{
				//if we can not find suitable sceneMgr ,we just use the base one
				it = mFactoryMap.find(SMT_GENERAL);
				mgr = it->second->createSceneMgr(name);
			}
			mSceneMgrMap[smt] = mgr;
			return mgr;
		}
	}
	//-------------------------------------------------------------//
	void SceneMgrSelector::destroySceneMgr(SceneMgr* mgr)
	{
		mSceneMgrMap.erase(mgr->getType());
	}
	//-------------------------------------------------------------//
	SceneMgr* SceneMgrSelector::getSceneMgr(SceneMgrType smt)
	{
		SceneMgrMap::iterator i = mSceneMgrMap.find(smt);
		if(i != mSceneMgrMap.end())
		{
			return i->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"this type of SceneMgr did not existed!!",
				"SceneMgrSelector::getSceneMgr");
			return 0;
		}
	}
	//-------------------------------------------------------------//
}