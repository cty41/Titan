#include "TitanStableHeader.h"
#include "Frustum.h"
#include "TiRenderer.h"
#include "Root.h"

namespace Titan
{
	const float Frustum::INFINITE_FAR_PLANE_ADJUST = 0.00001f;
	Frustum::Frustum()
		:mProjType(PT_PERSPECTIVE),  mOrientation(Quaternion::IDENTITY),
		mPosition(Vector3::ZERO), mUpdateView(true), mFarDist(3000.0f),
		mNearDist(1.0f), mAspect(4.0f/3.0f), mUpdateProj(true), mUpdateFrustumPlanes(true),
		mUpdateWorldSpaceCorners(true)
	{
		mFOVy = Radian(Math::PI/4.0f);
		mProjMatrixRS = Matrix4::ZERO;

		updateView();
		updateProjection();
	}
	//-------------------------------------------------------------------------------//
	Frustum::~Frustum()
	{
	
	}
	//-------------------------------------------------------------------------------//
	void Frustum::notifyViewUpdate()
	{
		mUpdateView = true;
		mUpdateWorldSpaceCorners = true;
		mUpdateFrustumPlanes = true;
	}
	//-------------------------------------------------------------------------------//
	bool Frustum::needUpdateView() const
	{
		return mUpdateView;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateView()
	{
		if(needUpdateView())
			updateViewImpl();

	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateViewImpl()
	{
		Matrix3 rot;
		const Quaternion& orientation = mOrientation;
		const Vector3& position = mPosition;

		mViewMatrix = Math::makeViewMatrix(position, orientation, 0);
		mUpdateView = false;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::notifyProjUpdate()
	{
		mUpdateProj = true;
		mUpdateFrustumPlanes = true;
		mUpdateWorldSpaceCorners = true;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateProjection()
	{
		if(needUpdateProj())
		{
			updateFrustumImpl();
		}
	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateFrustumImpl()
	{
		//calc the l,r,t,b
		float left, right, bottom, top;
		calcProjectionParams(left, right, bottom, top);

		float inv_w = 1 / (right - left);
		float inv_h = 1 / (top - bottom);
		float inv_d = 1 / (mFarDist - mNearDist);

		if(mProjType == PT_PERSPECTIVE)
		{
			float A = 2 * mNearDist * inv_w;
			float B = 2 * mNearDist * inv_h;
			float C = (right + left) * inv_w;
			float D = (top + bottom) * inv_h;
			float q, qn;

			if (mFarDist == 0)
			{
				// Infinite far plane
				q = Frustum::INFINITE_FAR_PLANE_ADJUST - 1;
				qn = mNearDist * (Frustum::INFINITE_FAR_PLANE_ADJUST - 2);
			}
			else
			{
				q = - (mFarDist + mNearDist) * inv_d;
				qn = -2 * (mFarDist * mNearDist) * inv_d;
			}

			// NB: This creates 'uniform' perspective projection matrix,
			// which depth range [-1,1], right-handed rules
			//
			// [ A   0   C   0  ]
			// [ 0   B   D   0  ]
			// [ 0   0   q   qn ]
			// [ 0   0   -1  0  ]
			//
			// A = 2 * near / (right - left)
			// B = 2 * near / (top - bottom)
			// C = (right + left) / (right - left)
			// D = (top + bottom) / (top - bottom)
			// q = - (far + near) / (far - near)
			// qn = - 2 * (far * near) / (far - near)

			mProjMatrix = Matrix4::ZERO;
			mProjMatrix[0][0] = A;
			mProjMatrix[0][2] = C;
			mProjMatrix[1][1] = B;
			mProjMatrix[1][2] = D;
			mProjMatrix[2][2] = q;
			mProjMatrix[2][3] = qn;
			mProjMatrix[3][2] = -1;

		}
		else if(mProjType == PT_ORTHOGRAPHIC)
		{

		}

		Renderer* renderer = Root::getSingletonPtr()->getActiveRenderer();
		renderer->_convertProjMatrix(mProjMatrix, mProjMatrixRS, true);


		float farDist = (mFarDist == 0) ? 100000 : mFarDist;
		// Near plane bounds
		Vector3 min(mLeft, mBottom, -farDist);
		Vector3 max(mRight, mTop, 0);

		if (mProjType == PT_PERSPECTIVE)
		{
			// Merge with far plane bounds
			float radio = farDist / mNearDist;
			min.makeFloor(Vector3(mLeft * radio, mBottom * radio, -farDist));
			max.makeCeil(Vector3(mRight * radio, mTop * radio, 0));
		}
		mAABB.setExtents(min, max);

		mUpdateProj = false;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::calcProjectionParams(float& left, float& right, float& bottom, float& top)
	{
		if(mProjType == PT_PERSPECTIVE)
		{
			Radian	thetaY(mFOVy * 0.5f);
			float tanThetaY = Math::Tan(thetaY);
			float tanThetaX = tanThetaY * mAspect;
			float half_w = tanThetaX * mNearDist;
			float half_h = tanThetaY * mNearDist;

			left	= -half_w;
			right	= +half_w;
			top	= +half_h;
			bottom	= -half_h;


			mLeft = left;
			mRight = right;
			mTop = top;
			mBottom = bottom;
		}
		else if(mProjType == PT_ORTHOGRAPHIC)
		{

		}
	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateFrustumPlanes()
	{
		updateView();
		updateProjection();

		if(mUpdateFrustumPlanes)
		{
			updateFrustumPlanesImpl();
		}
	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateFrustumPlanesImpl()
	{
		mProjViewMatrix = mProjMatrix * mViewMatrix;

		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = mProjViewMatrix[3][0] + mProjViewMatrix[0][0];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = mProjViewMatrix[3][1] + mProjViewMatrix[0][1];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = mProjViewMatrix[3][2] + mProjViewMatrix[0][2];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].d = mProjViewMatrix[3][3] + mProjViewMatrix[0][3];

		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = mProjViewMatrix[3][0] - mProjViewMatrix[0][0];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = mProjViewMatrix[3][1] - mProjViewMatrix[0][1];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = mProjViewMatrix[3][2] - mProjViewMatrix[0][2];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].d = mProjViewMatrix[3][3] - mProjViewMatrix[0][3];

		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.x = mProjViewMatrix[3][0] - mProjViewMatrix[1][0];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.y = mProjViewMatrix[3][1] - mProjViewMatrix[1][1];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.z = mProjViewMatrix[3][2] - mProjViewMatrix[1][2];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].d = mProjViewMatrix[3][3] - mProjViewMatrix[1][3];

		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = mProjViewMatrix[3][0] + mProjViewMatrix[1][0];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = mProjViewMatrix[3][1] + mProjViewMatrix[1][1];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = mProjViewMatrix[3][2] + mProjViewMatrix[1][2];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].d = mProjViewMatrix[3][3] + mProjViewMatrix[1][3];

		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = mProjViewMatrix[3][0] + mProjViewMatrix[2][0];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = mProjViewMatrix[3][1] + mProjViewMatrix[2][1];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = mProjViewMatrix[3][2] + mProjViewMatrix[2][2];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].d = mProjViewMatrix[3][3] + mProjViewMatrix[2][3];

		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.x = mProjViewMatrix[3][0] - mProjViewMatrix[2][0];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.y = mProjViewMatrix[3][1] - mProjViewMatrix[2][1];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.z = mProjViewMatrix[3][2] - mProjViewMatrix[2][2];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].d = mProjViewMatrix[3][3] - mProjViewMatrix[2][3];

		// Renormalise any normals which were not unit length
		for(int i=0; i<6; i++ ) 
		{
			float length = mFrustumPlanes[i].normal.normalise();
			mFrustumPlanes[i].d /= length;
		}

		mUpdateFrustumPlanes = false;
	}
	//-------------------------------------------------------------------------------//
	const Vector3* Frustum::getWorldSpaceCorners() 
	{
		updateWorldSpaceCorners();

		return mWorldSpaceCorners;
	}
	void Frustum::updateWorldSpaceCorners()
	{
		updateView();

		if(mUpdateWorldSpaceCorners)
			updateWorldSpaceCornersImpl();
	}
	//-------------------------------------------------------------------------------//
	void Frustum::updateWorldSpaceCornersImpl()
	{
		Matrix4 eyeToWorld = mViewMatrix.inverseAffine();

		// Note: Even though we can dealing with general projection matrix here,
		//       but because it's incompatibly with infinite far plane, thus, we
		//       still need to working with projection parameters.

		// Calc near plane corners
		float nearLeft, nearRight, nearBottom, nearTop;
		calcProjectionParams(nearLeft, nearRight, nearBottom, nearTop);

		// Treat infinite fardist as some arbitrary far value
		float farDist = (mFarDist == 0) ? 100000 : mFarDist;

		// Calc far palne corners
		float radio = mProjType == PT_PERSPECTIVE ? farDist / mNearDist : 1;
		float farLeft = nearLeft * radio;
		float farRight = nearRight * radio;
		float farBottom = nearBottom * radio;
		float farTop = nearTop * radio;

		// near
		mWorldSpaceCorners[0] = eyeToWorld.transformAffine(Vector3(nearRight, nearTop,    -mNearDist));
		mWorldSpaceCorners[1] = eyeToWorld.transformAffine(Vector3(nearLeft,  nearTop,    -mNearDist));
		mWorldSpaceCorners[2] = eyeToWorld.transformAffine(Vector3(nearLeft,  nearBottom, -mNearDist));
		mWorldSpaceCorners[3] = eyeToWorld.transformAffine(Vector3(nearRight, nearBottom, -mNearDist));
		// far
		mWorldSpaceCorners[4] = eyeToWorld.transformAffine(Vector3(farRight,  farTop,     -farDist));
		mWorldSpaceCorners[5] = eyeToWorld.transformAffine(Vector3(farLeft,   farTop,     -farDist));
		mWorldSpaceCorners[6] = eyeToWorld.transformAffine(Vector3(farLeft,   farBottom,  -farDist));
		mWorldSpaceCorners[7] = eyeToWorld.transformAffine(Vector3(farRight,  farBottom,  -farDist));

		for(int i = 0;i < 8; ++i)
			mWorldAABB.merge(mWorldSpaceCorners[i]);


		mUpdateWorldSpaceCorners = false;
	}
	//-------------------------------------------------------------------------------//
	const Matrix4& Frustum::getViewMatrix()
	{
		updateView();

		return mViewMatrix;
	}
	//-------------------------------------------------------------------------------//
	const Matrix4&	Frustum::getProjMatrix()
	{
		updateProjection();

		return mProjMatrix;
	}
	//-------------------------------------------------------------------------------//
	const Matrix4&	Frustum::getProjMatrixRS()
	{
		updateProjection();

		return mProjMatrixRS;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::setFOVy(const Radian& fovy)
	{
		mFOVy = fovy;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::setNearClipDistance(float nearDist)
	{
		mNearDist = nearDist;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::setFarClipDistance(float farDist)
	{
		mFarDist = farDist;
	}
	//-------------------------------------------------------------------------------//
	void Frustum::setAspectRatio(float ratio)
	{
		mAspect = ratio;
	}
	//-------------------------------------------------------------------------------//
	bool Frustum::isVisible(const AABB& aabb)
	{
		if (aabb.isNull()) return false;

		if(aabb.isInfinite()) return true;

		updateFrustumPlanes();

		Vector3 center = aabb.getCenter();
		Vector3 halfSize = aabb.getHalfSize();

		for(uint i = 0; i < 6; ++i)
		{
			if (i == FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;
			Plane::Side side = mFrustumPlanes[i].getSide(center, halfSize);
			if (side == Plane::NEGATIVE_SIDE)
				return false;
		}
		return true;
	}
	//-------------------------------------------------------------------------------//
	bool Frustum::isVisible(const Sphere& sphere)
	{
		updateFrustumPlanes();

		for(uint i = 0; i < 6; ++i)
		{
			if (i == FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;
			
			if (mFrustumPlanes[i].getDistance(sphere.getCenter()) < -sphere.getRadius())
				return false;
		}
		return true;
	}
	//-------------------------------------------------------------------------------//
	bool Frustum::isVisible(const Vector3& pos)
	{
        updateFrustumPlanes();

		for(uint i = 0; i < 6; ++i)
		{
			if (i == FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;
			if (mFrustumPlanes[i].getSide(pos) == Plane::NEGATIVE_SIDE)
				return false;
		}
		return true;
	}
}