#ifndef _TITAN_FRUSTUM_HH
#define _TITAN_FRUSTUM_HH

#include "TitanPrerequisites.h"

namespace Titan
{
	enum ProjectionType
	{
		PT_ORTHOGRAPHIC,
		PT_PERSPECTIVE
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

		const Matrix4&	getProjMatrix();

		const Matrix4&	getProjMatrixRS();
	
	protected:
		
		void	notifyProjUpdate();

		void	updateProjection();

		void	updateFrustumImpl();

		void	updateFrustumPlanes();

		void	calcProjectionParams();



	protected:
		ProjectionType	mProjType;
		Radian			mFOVy;
		float			mFarDist;
		float			mNearDist;
		float			mAspect;
		Matrix4			mProjMatrix;
		Matrix4			mProjMatrixRS;
		bool			mUpdateProj;
		float			mLeft, mRight, mTop, mBottom;
		Plane			mFrustumPlanes[6];

		static const float INFINITE_FAR_PLANE_ADJUST;

	};
}
#endif