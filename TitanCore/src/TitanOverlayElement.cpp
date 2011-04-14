#include "TitanStableHeader.h"
#include "TitanOverlayElement.h"
#include "TitanOverlayMgr.h"
#include "TiMaterialMgr.h"

namespace Titan
{
	OverlayElement::OverlayElement(const String& name)
		:mName(name), mMetricsMode(OMM_ABSOLUTE),mNeedUpdateGeometry(true), mNeedUpdateTexData(true),
		mIsInitialised(false),mAbsLeft(0.0f), mAbsTop(0.0f), mAbsWidth(1.0f), mAbsHeight(1.0f),
		mPixelScaleX(1.0f), mPixelScaleY(1.0f), mZOrder(0), mIsVisible(false)
	{
		mMaterial = MaterialMgr::getSingleton().getDefaultMaterial();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setName(const String& name)
	{
		if(mName != name)
		{
			OverlayMgr::getSingletonPtr()->renameElement(mName, name);
			mName = name;
		}
		
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setWidth(float width)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mAbsWidth = width;
		} 
		else
		{
			mWidth = width;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setHeight(float height)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mAbsHeight = height;
		} 
		else
		{
			mHeight = height;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setLeft(float left)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mAbsLeft = left;
		} 
		else
		{
			mLeft = left;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setTop(float top)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mAbsTop = top;
		} 
		else
		{
			mTop = top;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setSize(float width, float height)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mAbsHeight = height;
			mAbsWidth = width;
		}
		else
		{
			mWidth = width;
			mHeight = height;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setPosition(float left, float top)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mAbsLeft = left;
			mAbsTop = top;
		}
		else
		{
			mLeft = left;
			mTop = top;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::update()
	{

		if(mMetricsMode == OMM_ABSOLUTE)
		{
			if(OverlayMgr::getSingleton().hasViewportChanged() || mNeedUpdateGeometry)
			{
				float vpWidth, vpHeight;
				OverlayMgr& oMgr = OverlayMgr::getSingleton();
				vpWidth = (float) (oMgr.getViewportWidth());
				vpHeight = (float) (oMgr.getViewportHeight());

				mPixelScaleX = 1.0f / vpWidth;
				mPixelScaleY = 1.0f / vpHeight;

				mLeft = mAbsLeft * mPixelScaleX;
				mTop = mAbsTop * mPixelScaleY;
				mWidth = mAbsWidth * mPixelScaleX;
				mHeight = mAbsHeight * mPixelScaleY;
			}
		}
		else
		{

		}

		if(mNeedUpdateGeometry && mIsInitialised)
		{
			updateGeometryData();
			mNeedUpdateGeometry = false;
		}

		if(mNeedUpdateTexData && mIsInitialised)
		{
			updateTexData();
			mNeedUpdateTexData = false;
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::getTransformMat(Matrix4* transMat)
	{
		*transMat = Matrix4::IDENTITY;
	}
	//-------------------------------------------------------------------------------//
	float OverlayElement::getSquaredDistance(Camera* cam)
	{
		return 10000.0f;
	}
	//-------------------------------------------------------------------------------//
	void OverlayElement::setMetricsMode(OverlayMetricsMode mode)
	{
		if(mode == OMM_ABSOLUTE)
		{
			float vpWidth, vpHeight;
			OverlayMgr& oMgr = OverlayMgr::getSingleton();
			vpWidth = (float) (oMgr.getViewportWidth());
			vpHeight = (float) (oMgr.getViewportHeight());

			// cope with temporarily zero dimensions, avoid divide by zero
			vpWidth = vpWidth == 0.0f? 1.0f : vpWidth;
			vpHeight = vpHeight == 0.0f? 1.0f : vpHeight;

			mPixelScaleX = 1.0f / vpWidth;
			mPixelScaleY = 1.0f / vpHeight;

			if (mMetricsMode == OMM_RELATIVE)
			{
				mAbsLeft = mLeft;
				mAbsTop = mTop;
				mAbsWidth = mWidth;
				mAbsHeight = mHeight;
			}
		}
		else
		{
			mPixelScaleX = 1.0;
			mPixelScaleY = 1.0;
			mAbsLeft = mLeft;
			mAbsTop = mTop;
			mAbsWidth = mWidth;
			mAbsHeight = mHeight;
		}

		mLeft = mAbsLeft * mPixelScaleX;
		mTop = mAbsTop * mPixelScaleY;
		mWidth = mAbsWidth * mPixelScaleX;
		mHeight = mAbsHeight * mPixelScaleY;

		mMetricsMode = mode;
		notifyGeometryOld();
	}
}