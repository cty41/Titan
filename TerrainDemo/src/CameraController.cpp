#include "CameraController.h"

CameraController::CameraController(Titan::Camera* BaseCam)
:mCamera(BaseCam), mStyle(CS_FREE), mMaxSpeed(50.0f),
mForward(false), mBack(false), mLeft(false), mRight(false),
mBoost(false),mLeftButtonPressed(false), mRightButtonPressed(false)
,mVelocity(Titan::Vector3::ZERO)
{
}
//-------------------------------------------------------------------------------//
CameraController::~CameraController()
{
}
//-------------------------------------------------------------------------------//
void CameraController::update(float frameTime)
{
	if(mStyle == CS_FREE)
	{
		Titan::Vector3 accel = Titan::Vector3::ZERO;
		if(mLeft)		
			accel -= mCamera->getRight();
		if(mRight)		
			accel += mCamera->getRight();
		if(mForward)	
			accel += mCamera->getDirection();
		if(mBack)		
			accel -= mCamera->getDirection();

		float maxSpeed = mBoost ? mMaxSpeed * 20.0f : mMaxSpeed;
		if(accel.squaredLength() != 0.0f)
		{
			accel.normalise();
			mVelocity += accel * maxSpeed * frameTime;
		}
		else
			mVelocity -= mVelocity * frameTime * 10.0f;

		if (mVelocity.squaredLength() > maxSpeed * maxSpeed)
		{
			mVelocity.normalise();
			mVelocity *= maxSpeed;
		}
		else if (mVelocity.squaredLength() < 0.0f)
			mVelocity = Titan::Vector3::ZERO;

		if (mVelocity != Titan::Vector3::ZERO) mCamera->move(mVelocity * frameTime);
	}
}
//-------------------------------------------------------------------------------//
void CameraController::injectKeyPressd(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
	case OIS::KC_A:
		mLeft = true;
		break;
	case OIS::KC_D:
		mRight = true;
		break;
	case  OIS::KC_W:
		mForward = true;
		break;
	case OIS::KC_S:
		mBack = true;
		break;
	case OIS::KC_LSHIFT:
		mBoost = true;
		break;
	}
}
//-------------------------------------------------------------------------------//
void CameraController::injectKeyReleased(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
	case OIS::KC_A:
		mLeft = false;
		break;
	case OIS::KC_D:
		mRight = false;
	case  OIS::KC_W:
		mForward = false;
	case OIS::KC_S:
		mBack = false;
	case OIS::KC_LSHIFT:
		mBoost = false;
	}
}
//-------------------------------------------------------------------------------//
void CameraController::mouseMoved( const OIS::MouseEvent &arg )
{
	if(mRightButtonPressed&& mStyle == CS_FREE)
	{
		mCamera->yaw(Titan::Degree(-arg.state.X.rel * 0.15f));
		mCamera->pitch(Titan::Degree(-arg.state.Y.rel * 0.15f));
	}
}
//-------------------------------------------------------------------------------//
void CameraController::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mStyle == CS_FREE)
	{
		if(id == OIS::MB_Left)
			mLeftButtonPressed = true;
		else if(id == OIS::MB_Right)
			mRightButtonPressed = true;
	}
}
//-------------------------------------------------------------------------------//
void CameraController::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mStyle == CS_FREE)
	{
		if(id == OIS::MB_Left)
			mLeftButtonPressed = false;
		else if(id == OIS::MB_Right)
			mRightButtonPressed = false;
	}
}