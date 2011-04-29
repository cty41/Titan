#ifndef __TITAN_CAMERA_HH
#define __TITAN_CAMERA_HH

#include "TiPrerequisites.h"
#include "TiFrustum.h"

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

		const Vector3& getPosition();

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

		void setPolyMode(PolygonMode mode){ mPolygonMode = mode;}

		PolygonMode	getPolyMode() const { return mPolygonMode; }

		/** Internal method to notify camera of the visible faces in the last render.
        */
        void _notifyRenderedFaces(unsigned int numfaces);

        /** Internal method to notify camera of the visible batches in the last render.
        */
        void _notifyRenderedBatches(unsigned int numbatches);
        /** Internal method to retrieve the number of visible faces in the last render.
        */
		unsigned int _getNumRenderedFaces(void) const;

        /** Internal method to retrieve the number of visible batches in the last render.
        */
        unsigned int _getNumRenderedBatches(void) const;


	protected:


	protected:
		String			mName;
		SceneMgr*		mSceneMgr;
		PolygonMode		mPolygonMode;
        /// Stored number of visible faces in the last render
		uint			mVisFacesLastRender;
		/// Stored number of visible faces in the last render
		uint			mVisBatchesLastRender;

	};
}
#endif