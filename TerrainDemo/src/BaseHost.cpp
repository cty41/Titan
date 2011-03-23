#include "BaseHost.h"
#include "Timer.h"
#include "Camera.h"
#include "ConfigFile.h"
#include "TitanTextureMgr.h"
#include "TitanIteratorWrapper.h"
#include "ShaderEffectMgr.h"
#include "TitanQuadtreeSceneMgr.h"
#include "TitanBaseTerrain.h"


BaseHost::BaseHost()
:mRoot(0), mSceneMgr(0), mInputMgr(0),
mKeyboard(0), mMouse(0), mCamController(0), mTerrain(NULL)
{
	mTimer = TITAN_NEW Titan::Timer;
	mTimer->reset();
}
//-------------------------------------------------------------//
BaseHost::~BaseHost()
{
	destroy();
}
//-------------------------------------------------------------//
void BaseHost::destroy()
{
	if(mInputMgr)
	{
		if(mMouse)
			mInputMgr->destroyInputObject(mMouse);

		if(mKeyboard)
			mInputMgr->destroyInputObject(mKeyboard);

		OIS::InputManager::destroyInputSystem(mInputMgr);
	}

	if(mTerrain)
		TITAN_DELETE mTerrain;
	if(mCamController)
		TITAN_DELETE mCamController;

	if(mRoot)
	{
		TITAN_DELETE mRoot;
	}

}
//-------------------------------------------------------------//
void BaseHost::initResources()
{
	Titan::ConfigFile resConfig;
	resConfig.load("resources.cfg");

	Titan::ConfigFile::SectionMapIterator sit = resConfig.getSectionMapIterator();
	while (sit.hasMoreElements())
	{
		Titan::String group = sit.peekNextKey();
		Titan::ConfigFile::PropertyMapIterator pit = resConfig.getPropertyMapIterator(group);
		while (pit.hasMoreElements())
		{
			Titan::String type = pit.peekNextKey(), name = pit.peekNextValue();
			Titan::ResourceGroupManager::getSingltonPtr()->addResourceLocation(name, type, group, true);
			pit.next();
		}
		sit.next();
	}

	Titan::ResourceGroupManager::getSingltonPtr()->initResourceGroup(Titan::ResourceGroupManager::GENERAL_RESOURCE_GROUP);

}
//-------------------------------------------------------------//
void BaseHost::loadResources()
{
}
//-------------------------------------------------------------//
void BaseHost::initInputDevice()
{
	OIS::ParamList pl;
	size_t hwnd = 0;
	std::ostringstream windowHndStr;
	mWindow->getCustomParams("WINDOW", (void*)&hwnd);
	windowHndStr<<hwnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	mInputMgr = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard,true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);
}
//-------------------------------------------------------------//
void BaseHost::setup()
{
	mRoot = TITAN_NEW Titan::Root();
	mRoot->loadConfig();
	mWindow = mRoot->initialise();
	assert(mWindow);

	initResources();

	initInputDevice();

	mSceneMgr = mRoot->createSceneMgr("SceneMgr", Titan::SNT_QUADTREE);
	Titan::QuadtreeSceneMgr* quadMgr = static_cast<Titan::QuadtreeSceneMgr*>(mSceneMgr);
	quadMgr->setOctreeConfigs(Titan::AABB(-500.0f, 0, -500.0f,
										   500.0f, 500.0f, 500.0f));


	mCamera = mSceneMgr->createCamera("testCamera");
	mWindow->addViewport(mCamera);

	mCamController = TITAN_NEW CameraController(mCamera);



	Titan::Renderer* renderer = mRoot->getActiveRenderer();

	renderer->_setFillMode(Titan::PM_WIREFRAME);
	renderer->_setCullingMode(Titan::CULL_NONE);
	renderer->_setLightEnable(false);
#if 1
	Titan::TexturePtr pHeightmap = Titan::TextureMgr::getSingletonPtr()->createManually("heightMap1", Titan::ResourceGroupManager::GENERAL_RESOURCE_GROUP,
										Titan::TT_2D, 128, 128, 0,Titan::TU_DYNAMIC, Titan::PF_A8R8G8B8);

	pHeightmap->generatePerlinNoise(0.01f, 5, 0.6f);

	mTerrain = TITAN_NEW Titan::BaseTerrain();
	mTerrain->create(mSceneMgr->getRootSceneNode(), pHeightmap, Titan::AABB(-500.0f, 0, -500.0f, 500.0f, 500.0f, 500.0f));


	
#endif

#if 1
	Titan::ShaderEffectPtr pEffect = Titan::ShaderEffectMgr::getSingletonPtr()->loadManually("DefaultMesh.fx", Titan::ResourceGroupManager::GENERAL_RESOURCE_GROUP);
#endif

	mManualObject = mSceneMgr->createManualObject("test");
	mManualObject->begin();
	mManualObject->position(-1.0f,-1.0f,-1.0f);			//0
	mManualObject->position(-1.0f, 1.0f,-1.0f);			//1
	mManualObject->position( 1.0f, 1.0f,-1.0f);			//2
	mManualObject->position( 1.0f,-1.0f,-1.0f);			//3
	mManualObject->position(-1.0f,-1.0f, 1.0f);			//4
	mManualObject->position(-1.0f, 1.0f, 1.0f);			//5
	mManualObject->position( 1.0f, 1.0f, 1.0f);			//6
	mManualObject->position( 1.0f,-1.0f, 1.0f);			//7
	mManualObject->quad(0, 1, 2, 3);
	mManualObject->quad(4, 7, 6, 5);
	mManualObject->quad(4, 5, 1, 0);
	mManualObject->quad(3, 2, 6, 7);
	mManualObject->quad(1, 5, 6, 2);
	mManualObject->quad(4, 0, 3, 7);
	mManualObject->end();

	mManualObject->setShaderEffect(pEffect);

	Titan::SceneNode* node = mSceneMgr->getRootSceneNode()->createChild();
	node->rotate(Titan::Vector3::UNIT_X, Titan::Radian(3.14f * 0.25f));
	node->attachObject(mManualObject);

	mCamera->setPosition(0.0f, 0.0f, 10.0f);

}
//-------------------------------------------------------------//
void BaseHost::Run()
{
	mCurrTime = 0, mPreTime = 0;

	MSG msg;
	while (true)
	{
		if(PeekMessage(&msg,0,0, 0, PM_REMOVE) == TRUE)
		{
			if (msg.message == WM_QUIT || msg.message == WM_DESTROY )
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			updateOneFrame();
		}
	}
}
//-------------------------------------------------------------//
void BaseHost::updateOneFrame()
{
	mPreTime = mCurrTime;
	mCurrTime = mTimer->getMilliseconds();
	float timeDelta = mCurrTime - mPreTime;

	CaptureInput();
	updateLogic(timeDelta * 0.001f);
	render(timeDelta);
}
//-------------------------------------------------------------//
void BaseHost::CaptureInput()
{
	if(mKeyboard)
		mKeyboard->capture();

	if(mMouse)
		mMouse->capture();
}
//-------------------------------------------------------------//
void BaseHost::updateLogic(float frameTime)
{
	mCamController->update(frameTime);
}
//-------------------------------------------------------------//
void BaseHost::render(float frameTime)
{


	Titan::SceneNode* node = mManualObject->getAttachedNode();
	if(node)
	{
		// incremement y-rotation angle each frame
		node->rotate(Titan::Vector3::UNIT_Y, Titan::Radian(frameTime * 0.001f));
	}

	mRoot->renderOneFrame();
}
//-------------------------------------------------------------//
bool BaseHost::keyPressed(const OIS::KeyEvent &arg)
{

	if(mCamController)
		mCamController->injectKeyPressd(arg);
	return true;
}
//-------------------------------------------------------------//
bool BaseHost::keyReleased(const OIS::KeyEvent &arg)
{
	if(mCamController)
		mCamController->injectKeyReleased(arg);
	return true;
}
//-------------------------------------------------------------//
bool BaseHost::mouseMoved( const OIS::MouseEvent &arg )
{
	if(mCamController)
		mCamController->mouseMoved(arg);
	return true;
}
//-------------------------------------------------------------//
bool BaseHost::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mCamController)
		mCamController->mousePressed(arg, id);
	return true;
}
//-------------------------------------------------------------//
bool BaseHost::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mCamController)
		mCamController->mouseReleased(arg, id);
	return true;
}