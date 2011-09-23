#include "TitanStableHeader.h"
#include "TiRenderTarget.h"
#include "TiViewport.h"
#include "Timer.h"
#include "TiRoot.h"


namespace Titan
{
	RenderTarget::RenderTarget()
		: mPriority(0)
	{
		mTimer = Root::getSingleton().getTimer();
		resetStatistics();
	}
	//-------------------------------------------------------------------------------//
	RenderTarget::~RenderTarget()
	{
		removeAllViewports();
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::update()
	{
		_updateImpl();
	}
	//-------------------------------------------------------------------------------//
	Viewport* RenderTarget::addViewport(Camera* cam, int ZOrder, float left, float top ,
		float width, float height)
	{
		ViewportList::iterator it= mViewportList.find(ZOrder);
		if(it != mViewportList.end())
		{
			TITAN_EXCEPT_INVALIDPARAMS( 
				"We can not create this viewport 'cause its zorder has been used");
			return 0;
		}
		Viewport* vp = TITAN_NEW Viewport(cam, this,left, top, width, height, ZOrder);
		mViewportList.insert(ViewportList::value_type(ZOrder, vp));

		return vp;

	}
	//-------------------------------------------------------------------------------//
	Viewport* RenderTarget::getViewport(unsigned short index)
	{
		ViewportList::iterator it = mViewportList.find(index);
		if(it != mViewportList.end())
		{
			return it->second;
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"We do not have this viewport with this zorder");
			return 0;
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::removeViewport(int ZOrder)
	{
		ViewportList::iterator it = mViewportList.find(ZOrder);
		if(it != mViewportList.end())
		{
			mViewportList.erase(it);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"We do not have this viewport with this zorder, can not delete");
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::removeAllViewports()
	{
		mViewportList.clear();
	}
	//-------------------------------------------------------------------------------//
	ushort RenderTarget::getNumViewports() const
	{
		return mViewportList.size();
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::_beginUpdate()
	{
		mStats.triangleCount = 0;
		mStats.batchCount = 0;
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::_endUpdate()
	{
		updateStats();
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::_updateImpl()
	{
		_beginUpdate();
		_updateAutoUpdateViewports();
		_endUpdate();
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::_updateAutoUpdateViewports(bool updateStats)
	{
		ViewportList::iterator it = mViewportList.begin();
		while (it != mViewportList.end())
		{
			Viewport* viewport = (*it).second;
			if(viewport->isAutoUpdate())
				_updateViewport(viewport);
		
			++it;
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::_updateViewport(Viewport* vp, bool updateStats)
	{
		vp->update();
		if(updateStats)
		{
			mStats.triangleCount += vp->getNumRenderedFaces();
			mStats.batchCount += vp->getNumRenderedBatches();
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderTarget::getCustomParams(const String& name, void* pDst)
	 {
		 TITAN_EXCEPT_INTERNALERROR(" you have called the empty base function");
	 }
	//-------------------------------------------------------------------------------//
	void RenderTarget::getStatistics(float& lastFPS, float& avgFPS,
		float& bestFPS, float& worstFPS) const
	{

		// Note - the will have been updated by the last render
		lastFPS = mStats.lastFPS;
		avgFPS = mStats.avgFPS;
		bestFPS = mStats.bestFPS;
		worstFPS = mStats.worstFPS;
	}
	//-------------------------------------------------------------------------------//
	const RenderTarget::FrameStats& RenderTarget::getStatistics(void) const
	{
		return mStats;
	}

	float RenderTarget::getLastFPS() const
	{
		return mStats.lastFPS;
	}
	float RenderTarget::getAverageFPS() const
	{
		return mStats.avgFPS;
	}
	float RenderTarget::getBestFPS() const
	{
		return mStats.bestFPS;
	}
	float RenderTarget::getWorstFPS() const
	{
		return mStats.worstFPS;
	}

	size_t RenderTarget::getTriangleCount(void) const
	{
		return mStats.triangleCount;
	}

	size_t RenderTarget::getBatchCount(void) const
	{
		return mStats.batchCount;
	}

	float RenderTarget::getBestFrameTime() const
	{
		return (float)mStats.bestFrameTime;
	}

	float RenderTarget::getWorstFrameTime() const
	{
		return (float)mStats.worstFrameTime;
	}

	void RenderTarget::resetStatistics(void)
	{
		mStats.avgFPS = 0.0;
		mStats.bestFPS = 0.0;
		mStats.lastFPS = 0.0;
		mStats.worstFPS = 999.0;
		mStats.triangleCount = 0;
		mStats.batchCount = 0;
		mStats.bestFrameTime = 999999;
		mStats.worstFrameTime = 0;

		mLastTime = mTimer->getMilliseconds();
		mLastSecond = mLastTime;
		mFrameCount = 0;
	}

	void RenderTarget::updateStats(void)
	{
		++mFrameCount;
		unsigned long thisTime = mTimer->getMilliseconds();

		// check frame time
		unsigned long frameTime = thisTime - mLastTime ;
		mLastTime = thisTime ;

		mStats.bestFrameTime = std::min(mStats.bestFrameTime, frameTime);
		mStats.worstFrameTime = std::max(mStats.worstFrameTime, frameTime);

		// check if new second (update only once per second)
		if (thisTime - mLastSecond > 1000) 
		{ 
			// new second - not 100% precise
			mStats.lastFPS = (float)mFrameCount / (float)(thisTime - mLastSecond) * 1000.0f;

			if (mStats.avgFPS == 0)
				mStats.avgFPS = mStats.lastFPS;
			else
				mStats.avgFPS = (mStats.avgFPS + mStats.lastFPS) / 2; // not strictly correct, but good enough

			mStats.bestFPS = std::max(mStats.bestFPS, mStats.lastFPS);
			mStats.worstFPS = std::min(mStats.worstFPS, mStats.lastFPS);

			mLastSecond = thisTime ;
			mFrameCount  = 0;

		}

	}
}