#include "TitanStableHeader.h"
#include "Camera.h"


namespace Titan
{
	Camera::Camera(const String& name, SceneMgr* mgr)
		: Frustum(),mSceneMgr(mgr),
		mName(name), mOrientation(Quaternion::IDENTITY),
		mPosition(Vector3::ZERO), mUpdateView(true)
	{
		notifyViewUpdate();
		notifyProjUpdate();
	}
	//-------------------------------------------------------------//
	Camera::~Camera()
	{

	}
	//-------------------------------------------------------------//
	void Camera::setPosition(const Vector3& pos)
	{
		mPosition = pos;

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void Camera::setPosition(float x, float y, float z)
	{
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void Camera::move(const Vector3& vec)
	{
		mPosition += vec;

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void Camera::moveRelative(const Vector3& vec)
	{
		Vector3 trans = mOrientation * vec;

		mPosition = mPosition + trans;
		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void  Camera::setDirection(const Vector3& vec)
	{
		if (vec == Vector3::ZERO) return;

		// Remember, camera points down -Z of local axes!
		// Therefore reverse direction of direction vector before determining local Z
		Vector3 zAdjustVec = -vec;
		zAdjustVec.normalise();

		Quaternion targetWorldOrientation;


		// Get axes from current quaternion
		Vector3 axes[3];
		updateView();
		mOrientation.ToAxes(axes);
		Quaternion rotQuat;
		if ( (axes[2]+zAdjustVec).squaredLength() <  0.00005f) 
		{
			// Oops, a 180 degree turn (infinite possible rotation axes)
			// Default to yaw i.e. use current UP
			rotQuat.FromAngleAxis(Radian(Math::PI), axes[1]);
		}
		else
		{
			// Derive shortest arc to new direction
			rotQuat = axes[2].getRotationTo(zAdjustVec);

		}
		targetWorldOrientation = rotQuat * mOrientation;


		mOrientation = targetWorldOrientation;

		notifyViewUpdate();

	}
	//-------------------------------------------------------------//
	void Camera::setDirection(float x, float y, float z)
	{
		setDirection(Vector3(x, y, z));
	}
	//-------------------------------------------------------------//
	Vector3 Camera::getDirection() const
	{
		return mOrientation * -Vector3::UNIT_Z;
	}
	//-------------------------------------------------------------//
	Vector3 Camera::getUp() const
	{
		return mOrientation * Vector3::UNIT_Y;
	}
	//-------------------------------------------------------------//
	Vector3 Camera::getRight() const
	{
		return mOrientation * Vector3::UNIT_X;
	}
	//-------------------------------------------------------------//
	void Camera::lookAt( const Vector3& targetPoint )
	{
		setDirection(targetPoint - mPosition);
	}
	//-------------------------------------------------------------//
	void Camera::lookAt(float x, float y, float z)
	{
		setDirection(Vector3(x, y, z));
	}
	//-------------------------------------------------------------//
	void Camera::roll(const Radian& angle)
	{
		Vector3 zAxis = mOrientation * Vector3::UNIT_X;
		rotate(zAxis, angle);

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void Camera::yaw(const Radian& angle)
	{
		Vector3 yAxis = mOrientation * Vector3::UNIT_Y;
		rotate(yAxis, angle);

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void Camera::pitch(const Radian& angle)
	{
		Vector3 xAxis = mOrientation * Vector3::UNIT_X;
		rotate(xAxis, angle);

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	void Camera::rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.FromAngleAxis(angle,axis);
		rotate(q);
	}
	//-------------------------------------------------------------//
	void Camera::rotate(const Quaternion& q)
	{
		Quaternion qnorm = q;
		qnorm.normalise();
		mOrientation = qnorm * mOrientation;

		notifyViewUpdate();
	}
	//-------------------------------------------------------------//
	const Quaternion& Camera::getOrientation() const
	{
		return mOrientation;
	}
	//-------------------------------------------------------------//
	void Camera::setOrientation(const Quaternion& q)
	{
		mOrientation = q;
	}
	//-------------------------------------------------------------//
	void Camera::notifyViewUpdate()
	{
		mUpdateView = true;
	}
	//-------------------------------------------------------------//
	bool Camera::needUpdateView() const
	{
		return mUpdateView;
	}
	//-------------------------------------------------------------//
	void Camera::updateView()
	{
		if(needUpdateView())
			updateViewImpl();

	}
	//-------------------------------------------------------------//
	void Camera::updateViewImpl()
	{
		Matrix3 rot;
		const Quaternion& orientation = mOrientation;
		const Vector3& position = mPosition;

		mViewMatrix = Math::makeViewMatrix(position, orientation, 0);
		mUpdateView = false;
	}
	//-------------------------------------------------------------//
	const Matrix4& Camera::getViewMatrix()
	{
		updateView();

		return mViewMatrix;
	}
	//-------------------------------------------------------------//
	void Camera::_renderScene(Viewport* vp)
	{
		mSceneMgr->_renderScene(this, vp);
	}
}