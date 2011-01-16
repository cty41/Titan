#ifndef __BASEHOST__HH
#define __BASEHOST__HH

#include "Titan.h"
#include "OIS.h"

#include "CameraController.h"


class BaseHost : public OIS::KeyListener,public OIS::MouseListener, public Titan::GeneralAlloc
{
public:
	BaseHost();

	~BaseHost();

	void setup();

	void Run();

	void updateOneFrame();

	virtual void destroy();

	virtual void render(float frameTime);


protected:

	//we create buffed keyboard and mouse
	virtual void initInputDevice();

	void CaptureInput();

	void updateLogic(float frameTime);

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
	Titan::ManualObject*			mManualObject;
	Titan::Camera*					mCamera;
	CameraController*				mCamController;
	float							mCurrTime, mPreTime;

	OIS::InputManager*			mInputMgr;
	OIS::Mouse*					mMouse;
	OIS::Keyboard*				mKeyboard;
};
#endif
