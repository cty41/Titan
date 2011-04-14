#ifndef __TITANROOT_HH
#define __TITANROOT_HH

#include "StlHeader.h"
#include "TitanPrerequisites.h"
#include "Singleton.h"
#include "TitanCommon.h"
#include "SceneMgr.h"


namespace Titan
{
	typedef	std::vector<Renderer*>	RendererVector;
	typedef	std::vector<Plugin*>	PluginVector;
	typedef	std::vector<DynLib*>	DynLibVector;
	
	class _DllExport Root : public Singleton<Root>, public GeneralAlloc
	{
	public:
		Root(const String& configName = "config.cfg");

		~Root();
		
		RenderWindow*		initialise();

		SceneMgr*			createSceneMgr(const String& name, SceneMgrType smt = SMT_GENERAL);

		void				destroySceneMgr(SceneMgr* mgr);

		void				renderOneFrame();

		void				saveConfig();

		bool				loadConfig();

		void				addRenderer(Renderer* renderer);

		void				setActiveRenderer(Renderer* renderer);

		void				loadPlugin(const String& pluginName);

		void				unloadPlugin(const String& pluginName);

		void				installPlugin(Plugin* plugin);

		void				uninstallPlugin(Plugin* plugin);

		void				_updateAllTargets();

		Renderer*			getActiveRenderer() const { return mActiveRenderer; }

		void				addSceneObjectFactory(SceneObjectFactory* factory);

		void				removeAllSceneObjectFactory();

		SceneObjectFactory*	getSceneObjectFactory(const String& typeName);

		Timer*				getTimer() const { return mTimer; }

		void				convertColor(const Color& col, uint32* pDest);
	
		static Root*		getSingletonPtr();

		static Root&		getSingleton();
	
	protected:
		Renderer*			mActiveRenderer;

		RendererVector		mRendererVector;
		PluginVector		mPluginVector;
		DynLibVector		mDynLibVector;
		RenderWindow*		mWindow;
		ConsoleDebugger*	mConsoleDebugger;
		Timer*				mTimer;

		SceneMgrSelector*	mSceneMgrSelector;

		String				mConfigFileName;

		typedef map<String, SceneObjectFactory*>::type	SceneObjectFactoryMap;
		SceneObjectFactoryMap		mSceneObjectFactoryMap;

		FileSystemManager*		mFileSystemMgr;
		ResourceGroupMgr*		mResourceGroupMgr;
		MaterialMgr*			mMaterialMgr;
		OverlayMgr*				mOverlayMgr;

		FontMgr*				mFontMgr;


	};
}
#endif