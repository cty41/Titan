#ifndef __BASEHOST__HH
#define __BASEHOST__HH

#include "Titan.h"
#include "OIS.h"

#include "CameraController.h"


class BaseHost : public OIS::KeyListener,public OIS::MouseListener, public Titan::GeneralAlloc
{
public:
	BaseHost();

	virtual ~BaseHost();

	void setup();

	void Run();

	void updateOneFrame();

	virtual void destroy();

	virtual void render(float frameTime);


protected:
	virtual void initResources();

	virtual void loadResources();

	virtual void updateLogic(float frameTime);

	//we create buffed keyboard and mouse
	void initInputDevice();

	void captureInput();

	bool keyPressed(const OIS::KeyEvent &arg);

	bool keyReleased(const OIS::KeyEvent &arg);

	bool mouseMoved( const OIS::MouseEvent &arg );

	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	bool  mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

protected:
	Titan::Root*					mRoot;
	Titan::SceneMgr*				mSceneMgr;
	Titan::RenderWindow*			mWindow;
	Titan::Timer*					mTimer;
	Titan::Camera*					mCamera;
	CameraController*				mCamController;
	float							mCurrTime, mPreTime;
	Titan::OverlayTextElement*		mFPSLabel;

	OIS::InputManager*				mInputMgr;
	OIS::Mouse*						mMouse;
	OIS::Keyboard*					mKeyboard;
};
#endif
