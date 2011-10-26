#include "BaseHost.h"



BaseHost::BaseHost()
:mRoot(0), mSceneMgr(0), mInputMgr(0),
mKeyboard(0), mMouse(0), mCamController(0), mFPSLabel(NULL)
{

}
//-------------------------------------------------------------------------------//
BaseHost::~BaseHost()
{

}
//-------------------------------------------------------------------------------//
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

	if(mCamController)
		TITAN_DELETE mCamController;

	if(mRoot)
		TITAN_DELETE mRoot;
}
//-------------------------------------------------------------------------------//
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
			Titan::ResourceGroupMgr::getSingltonPtr()->addResourceLocation(name, type, group, true);
			pit.next();
		}
		sit.next();
	}
#if 1
	Titan::ResourceGroupMgr::getSingltonPtr()->initResourceGroup(Titan::ResourceGroupMgr::GENERAL_RESOURCE_GROUP);
#endif
}
//-------------------------------------------------------------------------------//
void BaseHost::loadResources()
{
}
//-------------------------------------------------------------------------------//
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
//-------------------------------------------------------------------------------//
void BaseHost::setup()
{
	mRoot = TITAN_NEW Titan::Root();
	mRoot->loadConfig();
	mTimer = mRoot->getTimer();
	mWindow = mRoot->initialise();
	assert(mWindow);
	initInputDevice();

	initResources();
	loadResources();

	Titan::Renderer* renderer = mRoot->getActiveRenderer();

	//renderer->_setPolygonMode(Titan::PM_WIREFRAME);
	renderer->_setCullingMode(Titan::CULL_NONE);


	//mFPSLabel = dynamic_cast<Titan::OverlayTextElement*>(Titan::OverlayMgr::getSingletonPtr()->createElement("OverlayTextElement", "testText"));
	//mFPSLabel->initialise("text", 0, 520, 14, Titan::OverlayTextElement::TA_Left);
}
//-------------------------------------------------------------------------------//
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
//-------------------------------------------------------------------------------//
void BaseHost::updateOneFrame()
{
	mPreTime = mCurrTime;
	mCurrTime = (float)mTimer->getMilliseconds();
	float timeDelta = mCurrTime - mPreTime;

	captureInput();
	updateLogic(timeDelta * 0.001f);
	render(timeDelta);

	Titan::RenderTarget::FrameStats stats = mWindow->getStatistics();
	std::ostringstream oss;
	Titan::String s;

	oss << "FPS: " << std::fixed << std::setprecision(1) << stats.lastFPS<<"\n"<<"triangleNum: "<<stats.triangleCount;

	s = oss.str();

	//mFPSLabel->setCaption(s);


}
//-------------------------------------------------------------------------------//
void BaseHost::captureInput()
{
	if(mKeyboard)
		mKeyboard->capture();

	if(mMouse)
		mMouse->capture();
}
//-------------------------------------------------------------------------------//
void BaseHost::updateLogic(float frameTime)
{
	mCamController->update(frameTime);
}
//-------------------------------------------------------------------------------//
void BaseHost::render(float frameTime)
{
	mRoot->renderOneFrame();
}
//-------------------------------------------------------------------------------//
bool BaseHost::keyPressed(const OIS::KeyEvent &arg)
{

	if(mCamController)
		mCamController->injectKeyPressd(arg);
	return true;
}
//-------------------------------------------------------------------------------//
bool BaseHost::keyReleased(const OIS::KeyEvent &arg)
{
	if(mCamController)
		mCamController->injectKeyReleased(arg);
	return true;
}
//-------------------------------------------------------------------------------//
bool BaseHost::mouseMoved( const OIS::MouseEvent &arg )
{
	if(mCamController)
		mCamController->mouseMoved(arg);
	return true;
}
//-------------------------------------------------------------------------------//
bool BaseHost::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mCamController)
		mCamController->mousePressed(arg, id);
	return true;
}
//-------------------------------------------------------------------------------//
bool BaseHost::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mCamController)
		mCamController->mouseReleased(arg, id);
	return true;
}