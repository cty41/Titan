#include "TitanStableHeader.h"
#include "Frustum.h"
#include "Renderer.h"
#include "Root.h"

namespace Titan
{
	const float Frustum::INFINITE_FAR_PLANE_ADJUST = 0.00001;
	Frustum::Frustum()
		:mProjType(PT_PERSPECTIVE), mFarDist(1000.0f),
		mNearDist(1.0f), mAspect(4.0f/3.0f), mUpdateProj(true)
	{
		mFOVy = Radian(Math::PI/4.0f);
		mProjMatrixRS = Matrix4::ZERO;

		updateProjection();
	}
	//-------------------------------------------------------------//
	Frustum::~Frustum()
	{
	
	}
	//-------------------------------------------------------------//
	void Frustum::notifyProjUpdate()
	{
		mUpdateProj = true;
	}
	//-------------------------------------------------------------//
	void Frustum::updateProjection()
	{
		if(needUpdateProj())
		{
			updateFrustumImpl();
		}
	}
	//-------------------------------------------------------------//
	void Frustum::updateFrustumImpl()
	{
		//calc the l,r,t,b
		calcProjectionParams();

		float inv_w = 1 / (mRight - mLeft);
		float inv_h = 1 / (mTop - mBottom);
		float inv_d = 1 / (mFarDist - mNearDist);

		if(mProjType == PT_PERSPECTIVE)
		{
			float A = 2 * mNearDist * inv_w;
			float B = 2 * mNearDist * inv_h;
			float C = (mRight + mLeft) * inv_w;
			float D = (mTop + mBottom) * inv_h;
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
		renderer->_convertProjMatrix(mProjMatrix, mProjMatrixRS);

		mUpdateProj = false;
	}
	//-------------------------------------------------------------//
	void Frustum::calcProjectionParams()
	{
		if(mProjType == PT_PERSPECTIVE)
		{
			Radian	thetaY(mFOVy * 0.5f);
			float tanThetaY = Math::Tan(thetaY);
			float tanThetaX = tanThetaY * mAspect;
			float half_w = tanThetaX * mNearDist;
			float half_h = tanThetaY * mNearDist;

			mLeft	= -half_w;
			mRight	= +half_w;
			mTop	= +half_h;
			mBottom	= -half_h;
		}
		else if(mProjType == PT_ORTHOGRAPHIC)
		{

		}
	}
	//-------------------------------------------------------------//
	const Matrix4&	Frustum::getProjMatrix()
	{
		updateProjection();

		return mProjMatrix;
	}
	//-------------------------------------------------------------//
	const Matrix4&	Frustum::getProjMatrixRS()
	{
		updateProjection();

		return mProjMatrixRS;
	}
	//-------------------------------------------------------------//
	void Frustum::setFOVy(const Radian& fovy)
	{
		mFOVy = fovy;
	}
	//-------------------------------------------------------------//
	void Frustum::setNearClipDistance(float nearDist)
	{
		mNearDist = nearDist;
	}
	//-------------------------------------------------------------//
	void Frustum::setFarClipDistance(float farDist)
	{
		mFarDist = farDist;
	}
	//-------------------------------------------------------------//
	void Frustum::setAspectRatio(float ratio)
	{
		mAspect = ratio;
	}

}