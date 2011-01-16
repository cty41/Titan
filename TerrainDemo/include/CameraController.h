#ifndef __TITAN_CameraController_HH
#define __TITAN_CameraController_HH

#include "Titan.h"
#include "OIS.h"

enum CameraStyle
{
	CS_FREE = 0
};

class  CameraController: public Titan::GeneralAlloc
{
public:
	CameraController(Titan::Camera* BaseCam);

	~CameraController();

	void update(float frameTime);

	void injectKeyPressd(const OIS::KeyEvent &arg);

	void injectKeyReleased(const OIS::KeyEvent &arg);

	void mouseMoved( const OIS::MouseEvent &arg );

	void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id );

protected:
	Titan::Camera*			mCamera;
	CameraStyle				mStyle;
	Titan::Vector3			mVelocity;
	float					mMaxSpeed;
	bool					mForward;
	bool					mBack;
	bool					mLeft;
	bool					mRight;
	bool					mBoost;
	bool					mLeftButtonPressed;
	bool					mRightButtonPressed;
};

#endif