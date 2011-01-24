#include "TitanStableHeader.h"
#include "Root.h"
#include "ConsoleDebugger.h"
#include "TitanPlugin.h"
#include "TitanDynLib.h"
#include "SceneMgrSelector.h"
#include "ManualObject.h"
#include "CommonFileSystem.h"
#include "TitanResourceGroupManager.h"

namespace Titan
{
	template<> Root* Singleton<Root>::ms_Singleton = 0;

	Root* Root::getSingletonPtr()
	{
		return ms_Singleton;
	}
	Root& Root::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	typedef void (*DLL_START_PLUGIN)(void);
	typedef void (*DLL_STOP_PLUGIN)(void);

	//-------------------------------------------------------------//
	Root::Root(const String& configName)
		:mWindow(0), mConsoleDebugger(0), mActiveRenderer(0),
		mConfigFileName(configName), mSceneMgrSelector(0), mFileSystemMgr(0),
		mResourceGroupMgr(0)
	{
		mConsoleDebugger = TITAN_NEW ConsoleDebugger();

		mSceneMgrSelector = TITAN_NEW SceneMgrSelector();

		addSceneObjectFactory(TITAN_NEW ManualObjectFactory());

//#ifdef DEBUG
		loadPlugin("Plugin_D3D9Renderer_d.dll");
//#else
//		loadPlugin("Plugin_D3D9Renderer.dll");
//#endif
		mActiveRenderer = mRendererVector[0];

		mFileSystemMgr = TITAN_NEW FileSystemManager();
		mFileSystemMgr->addFileSystemFactory(TITAN_NEW CommonFileSystemFactory());

		mResourceGroupMgr = TITAN_NEW ResourceGroupManager();

	}
	//-------------------------------------------------------------//
	Root::~Root()
	{
		TITAN_DELETE mResourceGroupMgr;
		TITAN_DELETE mFileSystemMgr;

		TITAN_DELETE mSceneMgrSelector;
		removeAllSceneObjectFactory();

		unloadPlugin("Plugin_D3D9Renderer_d.dll");
		TITAN_DELETE mConsoleDebugger;
	}
	//-------------------------------------------------------------//
	RenderWindow* Root::initialise()
	{
		mWindow = mActiveRenderer->initialize();
		return mWindow;
	}
	//-------------------------------------------------------------//
	void Root::renderOneFrame()
	{
		_updateAllTargets();
	}
	//-------------------------------------------------------------//
	SceneMgr* Root::createSceneMgr(const String& name, SceneMgrType smt)
	{
		return mSceneMgrSelector->createSceneMgr(name, smt);
	}
	//-------------------------------------------------------------//
	void Root::destroySceneMgr(SceneMgr* mgr)
	{
		if(mgr)
			mSceneMgrSelector->destroySceneMgr(mgr);
	}
	//-------------------------------------------------------------//
	void Root::saveConfig()
	{
		if(mConfigFileName.empty())
			return;

		std::ofstream of(mConfigFileName.c_str());
		
		if(of == 0)
			TITAN_EXCEPT(Exception::EXCEP_CANNOT_WRITE_TO_FILE, "can not write config text", "Root::saveConfig()");
	

		const ConfigOptionMap& com = mActiveRenderer->getConfigOptionMap();
		for(ConfigOptionMap::const_iterator it = com.begin();it != com.end(); ++it)
		{
			of << it->first << "=" << it->second.value<<std::endl;
		}

		of.close();

	}
	//-------------------------------------------------------------//
	bool Root::loadConfig()
	{
		if(mConfigFileName.empty())
			return true;

		std::ifstream fp;

		fp.open(mConfigFileName.c_str(), std::ios::in | std::ios::binary );

		if(fp == 0)
		{
			ConsoleDebugger::getSingleton().stream() <<"'" <<mConfigFileName<< "' not found, create and use default config.";
			return false;
		}


		String line, first, second;
		while(!fp.eof())
		{
			getline(fp, line);
			if(line.length() > 0 && line.find('=') != String::npos && line.at(0) != '#')
			{
				first = line.substr(0,line.find('='));
				second = line.substr(line.find('=') + 1, line.length() - line.find('=') - 2);
				mActiveRenderer->setConfigOption(first, second);
			}
		}
		fp.close();

		return true;
	}
	//-------------------------------------------------------------//
	void Root::addRenderer(Renderer* renderer)
	{
		mRendererVector.push_back(renderer);
	}
	void Root::setActiveRenderer(Renderer* renderer)
	{
		if(mActiveRenderer != 0 && mActiveRenderer != renderer)
		{
			mActiveRenderer->destroy();
		}

		mActiveRenderer = renderer;

	}
	//-------------------------------------------------------------//
	void Root::installPlugin(Plugin* plugin)
	{
		ConsoleDebugger::getSingleton().stream()
			<<"Installing plugin: "<<plugin->getName();
		mPluginVector.push_back(plugin);
		plugin->install();

		plugin->initialise();
		ConsoleDebugger::getSingleton().stream()
			<<"Plugin successfully installed";
	}
	//-------------------------------------------------------------//
	void Root::uninstallPlugin(Plugin* plugin)
	{
		ConsoleDebugger::getSingleton().stream()
			<<"Uninstalling plugin: "<<plugin->getName();
		PluginVector::iterator i = 
			std::find(mPluginVector.begin(), mPluginVector.end(), plugin);

		if(i != mPluginVector.end())
		{
			plugin->shutdown();
			plugin->uninstall();
			mPluginVector.erase(i);
		}
		ConsoleDebugger::getSingleton().stream()
			<<"Plugin successfully uninstalled";

	}
	//-------------------------------------------------------------//
	void Root::loadPlugin(const String& pluginName)
	{
		 DynLib* lib = TITAN_NEW DynLib(pluginName);
		 lib->load();

		 if (std::find(mDynLibVector.begin(), mDynLibVector.end(), lib) == mDynLibVector.end())
		 {
			 mDynLibVector.push_back(lib);

			 // Call startup function
			 DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("dllStartPlugin");

			 if (!pFunc)
				 TITAN_EXCEPT(Exception::EXCEP_FILE_NOT_FOUND, "Cannot find symbol dllStartPlugin in library " + pluginName,
				 "Root::loadPlugin");

			 // This must call installPlugin
			 pFunc();
		 }
	}
	//-------------------------------------------------------------//
	void Root::unloadPlugin(const String& pluginName)
	{
		DynLibVector::iterator i;

		for (i = mDynLibVector.begin(); i != mDynLibVector.end(); ++i)
		{
			if ((*i)->getName() == pluginName)
			{
				// Call plugin shutdown
				DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*i)->getSymbol("dllStopPlugin");
				// this must call uninstallPlugin
				pFunc();
				// Unload library (destroyed by DynLibManager)
				(*i)->unload();
				mDynLibVector.erase(i);
				return;
			}

		}
	}
	//-------------------------------------------------------------//
	void Root::_updateAllTargets()
	{
		 mActiveRenderer->updateAllTargets();

		 mActiveRenderer->swapAllTargetBuffers();
	}
	//-------------------------------------------------------------//
	void Root::addSceneObjectFactory(SceneObjectFactory* factory)
	{
		SceneObjectFactoryMap::iterator it = mSceneObjectFactoryMap.find(factory->getType());

		if(it == mSceneObjectFactoryMap.end())
		{
			mSceneObjectFactoryMap.insert(
				SceneObjectFactoryMap::value_type(factory->getType(),
							factory));
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"SceneObjectFactory called " + factory->getType() + " has existed!!",
				"Root::addSceneObjectFactory");
		}
	}
	//-------------------------------------------------------------//
	void Root::removeAllSceneObjectFactory()
	{
		SceneObjectFactoryMap::iterator it = mSceneObjectFactoryMap.begin(),
			itend = mSceneObjectFactoryMap.end();
		while(it != itend)
		{
			TITAN_DELETE it->second;
			++it;
		}
		mSceneObjectFactoryMap.clear();
	}
	//-------------------------------------------------------------//
	SceneObjectFactory* Root::getSceneObjectFactory(const String& typeName)
	{
		SceneObjectFactoryMap::iterator it = mSceneObjectFactoryMap.find(typeName);

		if(it != mSceneObjectFactoryMap.end())
		{
			return it->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"SceneObjectFactory called " + typeName + " does not exist!!",
				"Root::getSceneObjectFactory");
			return 0;
		}
	}
	

}