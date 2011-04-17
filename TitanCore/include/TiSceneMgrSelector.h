#ifndef _TITAN_SCENEMGR_SELECTOR_HH
#define _TITAN_SCENEMGR_SELECTOR_HH

#include "TiPrerequisites.h"
#include "TiSingleton.h"
#include "TiSceneMgr.h"

namespace Titan
{
	class _DllExport SceneMgrSelector : public Singleton<SceneMgrSelector> , public GeneralAlloc
	{
	public:
		SceneMgrSelector();

		~SceneMgrSelector();

		void	addFactory(SceneMgrFactory* factory);

		void	removeFactory(SceneMgrFactory* factory);

		SceneMgr* createSceneMgr(const String& name, SceneMgrType smt);

		void  destroySceneMgr(SceneMgr* mgr);

		SceneMgr*	getSceneMgr(SceneMgrType smt);

		static SceneMgrSelector*	getSingletonPtr();

		static SceneMgrSelector&	getSingleton();
	protected:
		
		typedef map<SceneMgrType, SceneMgrFactory*>::type FactoryMap;
		FactoryMap		mFactoryMap;
		typedef map<SceneMgrType, SceneMgr*>::type SceneMgrMap;
		SceneMgrMap		mSceneMgrMap;
	};
}

#endif