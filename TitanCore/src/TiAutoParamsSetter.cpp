#include "TitanStableHeader.h"
#include "TiAutoParamsSetter.h"
#include "TiRenderable.h"
#include "TiPass.h"
#include "TiCamera.h"
#include "TiRoot.h"
#include "TiRenderer.h"

namespace Titan
{
	AutoParamsSetter::AutoParamsSetter()
		:mCurrentCam(NULL), mCurrentPass(NULL), mCurrentRenderable(NULL),
		mWorldMatrixDirty(true), mViewMatrixDirty(true), mProjMatrixDirty(true),
		mViewProjMatrixDirty(true), mWorldViewMatrixDirty(true), mWorldViewProjMatrixDirty(true)
	{

	}
	//------------------------------------------------------------------------------//
	inline void AutoParamsSetter::setCurrentRenderable(Renderable* rend)
	{
		mCurrentRenderable = rend;
		mWorldMatrixDirty = true;
		mWorldViewMatrixDirty = true;
		mWorldViewProjMatrixDirty = true;
	}
	//------------------------------------------------------------------------------//
	inline void AutoParamsSetter::setCurrentPass(Pass* pass)
	{
		mCurrentPass = pass;
	}
	//------------------------------------------------------------------------------//
	void AutoParamsSetter::setCurrentCamera(Camera* cam)
	{
		mCurrentCam = cam;
		mViewMatrixDirty = true;
		mViewProjMatrixDirty = true;
		mProjMatrixDirty = true;
	}
	//------------------------------------------------------------------------------//
	void AutoParamsSetter::setWorldMatrix(const Matrix4& mat)
	{
		mWorldMatrix = mat;
		mWorldMatrixDirty = false;
	}
	//------------------------------------------------------------------------------//
	const Matrix4&	AutoParamsSetter::getWorldMatrix()
	{
		if (mWorldMatrixDirty)
		{
			mCurrentRenderable->getTransformMat(&mWorldMatrix);
			mWorldMatrixDirty = false;
		}
		return mWorldMatrix;
	}
	//------------------------------------------------------------------------------//
	const Matrix4&	AutoParamsSetter::getViewMatrix()
	{
		if (mViewMatrixDirty)
		{
			if(mCurrentRenderable && mCurrentRenderable->IsUseIdentityView())
				mViewMatrix = Matrix4::IDENTITY;
			else
			{
				mViewMatrix = mCurrentCam->getViewMatrix();
			}

			mViewMatrixDirty = false;
		}
		return mViewMatrix;
	}
	//------------------------------------------------------------------------------//
	const Matrix4&	AutoParamsSetter::getProjMatrix()
	{
		if (mProjMatrixDirty)
		{
			if (mCurrentRenderable && mCurrentRenderable->IsUseIdentityProj())
			{
				Root::getSingleton().getActiveRenderer()->_convertProjMatrix(Matrix4::IDENTITY, mProjMatrix, true);
			}
			else
			{
				mProjMatrix = mCurrentCam->getProjMatrix();

			}
			mProjMatrixDirty = false;
		}
		return mProjMatrix;
	}
	//------------------------------------------------------------------------------//
	const Matrix4&	AutoParamsSetter::getViewProjMatrix()
	{
		if (mViewProjMatrixDirty)
		{
			mViewProjMatrix = mCurrentCam->getProjMatrix() * mCurrentCam->getViewMatrix();
			mViewProjMatrixDirty = false;
		}
		return mViewProjMatrix;
	}
	//------------------------------------------------------------------------------//
	const Matrix4&	AutoParamsSetter::getWorldViewMatrix()
	{
		if(mWorldViewMatrixDirty)
		{
			mWorldViewMatrix = getViewMatrix() * getWorldMatrix();
			mWorldMatrixDirty = false;
		}
		return mWorldViewMatrix;
	}
	//------------------------------------------------------------------------------//
	const Matrix4&	AutoParamsSetter::getWorldViewProjMatrix()
	{
		if (mWorldViewProjMatrixDirty)
		{
			mWorldViewProjMatrix = getProjMatrix() * getWorldViewMatrix();
			mWorldViewProjMatrixDirty = false;
		}
		return mWorldViewProjMatrix;
	}
}