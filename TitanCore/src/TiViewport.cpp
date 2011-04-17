#include "TitanStableHeader.h"
#include "TiViewport.h"
#include "TiRenderTarget.h"
#include "TiCamera.h"

namespace Titan
{
	Viewport::Viewport(Camera* camera, RenderTarget* target, 
		float left, float top, float width, float height, int ZOrder)
		: mCamera(camera), mTarget(target), 
		mRelLeft(left), mRelTop(top), mRelWidth(width), 
		mRelHeight(height), mZOrder(ZOrder),
		mIsAutoUpdate(true),mClearBuffers(FBT_COLOR | FBT_DEPTH),
		mBackColor(Color::Grey)
	{
		_updateDimensions();
	}
	//-------------------------------------------------------------------------------//
	Viewport::~Viewport()
	{

	}
	//-------------------------------------------------------------------------------//
	void Viewport::_updateDimensions(void)
	{
		float height = (float) mTarget->getHeight();
		float width = (float) mTarget->getWidth();

		mActLeft = (int) (mRelLeft * width);
		mActTop = (int) (mRelTop * height);
		mActWidth = (int) (mRelWidth * width);
		mActHeight = (int) (mRelHeight * height);

		if (mCamera) 
			mCamera->setAspectRatio((float) mActWidth / (float) mActHeight);
	}
	//-------------------------------------------------------------------------------//
	int Viewport::getZOrder(void) const
	{
		return mZOrder;
	}
	//-------------------------------------------------------------------------------//
	RenderTarget* Viewport::getTarget(void) const
	{
		return mTarget;
	}
	//-------------------------------------------------------------------------------//
	Camera* Viewport::getCamera(void) const
	{
		return mCamera;
	}
	//-------------------------------------------------------------------------------//
	float Viewport::getLeft(void) const
	{
		return mRelLeft;
	}
	//-------------------------------------------------------------------------------//
	float Viewport::getTop(void) const
	{
		return mRelTop;
	}
	//-------------------------------------------------------------------------------//
	float Viewport::getWidth(void) const
	{
		return mRelWidth;
	}
	//-------------------------------------------------------------------------------//
	float Viewport::getHeight(void) const
	{
		return mRelHeight;
	}
	//-------------------------------------------------------------------------------//
	int Viewport::getActualLeft(void) const
	{
		return mActLeft;
	}
	//-------------------------------------------------------------------------------//
	int Viewport::getActualTop(void) const
	{
		return mActTop;
	}
	//-------------------------------------------------------------------------------//
	int Viewport::getActualWidth(void) const
	{
		return mActWidth;
	}
	//-------------------------------------------------------------------------------//
	int Viewport::getActualHeight(void) const
	{
		return mActHeight;
	}
	//-------------------------------------------------------------------------------//
	void Viewport::setDimensions(float left, float top, float width, float height)
	{
		mRelLeft = left;
		mRelTop = top;
		mRelWidth = width;
		mRelHeight = height;
		_updateDimensions();
	}
	//-------------------------------------------------------------------------------//
	void Viewport::update()
	{
		if (mCamera)
		{
			// Tell Camera to render into me
			mCamera->_renderScene(this);
		}
	}
	//-------------------------------------------------------------------------------//
	void Viewport::setBackgroundColor(const Color& color)
	{
		mBackColor = color;
	}
	//-------------------------------------------------------------------------------//
	const Color& Viewport::getBackgroundColor(void) const
	{
		return mBackColor;
	}
	//-------------------------------------------------------------------------------//
	unsigned int Viewport::getNumRenderedFaces(void) const
	{
		return mCamera ? mCamera->_getNumRenderedFaces() : 0;
	}
	//-------------------------------------------------------------------------------//
	unsigned int Viewport::getNumRenderedBatches(void) const
	{
		return mCamera ? mCamera->_getNumRenderedBatches() : 0;
	}
}