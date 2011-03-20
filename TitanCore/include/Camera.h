#ifndef __TITAN_CAMERA_HH
#define __TITAN_CAMERA_HH

#include "TitanPrerequisites.h"
#include "Frustum.h"

namespace Titan
{
	class _DllExport Camera : public Frustum
	{
	public:
		Camera(const String& name, SceneMgr* mgr);

		virtual ~Camera();

		const String& getName() const{ return mName; }

		void	setPosition(float x, float y, float z);

		void	setPosition(const Vector3& pos);

		const Vector3& getPosition() const { return mPosition; }

		void move(const Vector3& vec);

        void moveRelative(const Vector3& vec);

        void setDirection(float x, float y, float z);

        void setDirection(const Vector3& vec);

        Vector3 getDirection() const;

        Vector3 getUp() const;

        Vector3 getRight() const;

		void lookAt( const Vector3& targetPoint );

        void lookAt(float x, float y, float z);

        void roll(const Radian& angle);

        void yaw(const Radian& angle);

        void pitch(const Radian& angle);

        void rotate(const Vector3& axis, const Radian& angle);

        void rotate(const Quaternion& q);

        const Quaternion& getOrientation() const;

        void setOrientation(const Quaternion& q);

		void _renderScene(Viewport* vp);


	protected:


	protected:
		String			mName;
		SceneMgr*		mSceneMgr;

	};
}
#endif