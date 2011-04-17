#ifndef _TITAN_FRUSTUM_HH
#define _TITAN_FRUSTUM_HH

#include "TiPrerequisites.h"
#include "TitanAABB.h"
#include "TitanSphere.h"

namespace Titan
{
	enum ProjectionType
	{
		PT_ORTHOGRAPHIC,
		PT_PERSPECTIVE
	};

	enum FrustumPlane
	{
		FRUSTUM_PLANE_NEAR   = 0,
		FRUSTUM_PLANE_FAR    = 1,
		FRUSTUM_PLANE_LEFT   = 2,
		FRUSTUM_PLANE_RIGHT  = 3,
		FRUSTUM_PLANE_TOP    = 4,
		FRUSTUM_PLANE_BOTTOM = 5
	};

	class _DllExport Frustum : public GeneralAlloc
	{
	public:
		Frustum();

		~Frustum();

		const Matrix4& getViewMatrix() const;

		void setFOVy(const Radian& fovy);

		const Radian& getFOVy() const{ return mFOVy; }

		void setNearClipDistance(float nearDist);

		float getNearClipDistance() const { return mNearDist; }

		void setFarClipDistance(float nearDist);

		float getFarClipDistance() const { return mFarDist; }

		void setAspectRatio(float ratio);

		float getAspectRatio() const{ return mAspect; }

		bool	needUpdateProj() const { return mUpdateProj;}

		const Matrix4&	getViewMatrix();

		const Matrix4&	getProjMatrix();

		const Matrix4&	getProjMatrixRS();

		bool	isVisible(const AABB& aabb);

		bool	isVisible(const Sphere& sphere);

		bool	isVisible(const Vector3& pos);

		const AABB& getAABB() const { return mAABB; }

		const AABB& getWorldAABB() { updateWorldSpaceCorners(); return mWorldAABB; }
	
	protected:
		void	notifyViewUpdate();

		bool	needUpdateView() const ;

		void	updateView();

		void	updateViewImpl();

		void	notifyProjUpdate();

		void	updateProjection();

		void	updateFrustumImpl();

		void	updateFrustumPlanes();

		void	updateFrustumPlanesImpl();

		void	calcProjectionParams(float& left, float& right, float& bottom, float& top);

		void	updateWorldSpaceCorners();

		void	updateWorldSpaceCornersImpl();

		const Vector3* getWorldSpaceCorners();



	protected:
		ProjectionType	mProjType;
		Quaternion		mOrientation;
		Vector3			mPosition;
		AABB			mAABB;		//local
		AABB			mWorldAABB;	
		Radian			mFOVy;
		float			mFarDist;
		float			mNearDist;
		float			mAspect;
		bool			mUpdateView;
		Matrix4			mViewMatrix;
		Matrix4			mProjMatrix;
		Matrix4			mProjMatrixRS;
		Matrix4			mProjViewMatrix;
		bool			mUpdateProj;
		float			mLeft, mRight, mTop, mBottom;
		Plane			mFrustumPlanes[6];
		bool			mUpdateFrustumPlanes;
		Vector3			mWorldSpaceCorners[8];
		bool			mUpdateWorldSpaceCorners;

		static const float INFINITE_FAR_PLANE_ADJUST;

	};
}
#endif