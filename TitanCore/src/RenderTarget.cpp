#include "TitanStableHeader.h"
#include "RenderTarget.h"
#include "Viewport.h"



namespace Titan
{
	RenderTarget::RenderTarget()
		: mPriority(0)
	{
	}
	//-------------------------------------------------------------//
	RenderTarget::~RenderTarget()
	{
		removeAllViewports();
	}
	//-------------------------------------------------------------//
	void RenderTarget::update()
	{
		_updateImpl();
	}
	//-------------------------------------------------------------//
	Viewport* RenderTarget::addViewport(Camera* cam, int ZOrder, float left, float top ,
		float width, float height)
	{
		ViewportList::iterator it= mViewportList.find(ZOrder);
		if(it != mViewportList.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"We can not create this viewport 'cause its zorder has been used",
				"RenderTarget::addViewport");
			return 0;
		}
		Viewport* vp = TITAN_NEW Viewport(cam, this, ZOrder, left, top, width, height);
		mViewportList.insert(ViewportList::value_type(ZOrder, vp));

		return vp;

	}
	//-------------------------------------------------------------//
	Viewport* RenderTarget::getViewport(unsigned short index)
	{
		ViewportList::iterator it = mViewportList.find(index);
		if(it != mViewportList.end())
		{
			return it->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"We do not have this viewport with this zorder",
				"RenderTarget::getViewport");
			return 0;
		}
	}
	//-------------------------------------------------------------//
	void RenderTarget::removeViewport(int ZOrder)
	{
		ViewportList::iterator it = mViewportList.find(ZOrder);
		if(it != mViewportList.end())
		{
			mViewportList.erase(it);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"We do not have this viewport with this zorder, can not delete",
				"RenderTarget::removeViewport");
		}
	}
	//-------------------------------------------------------------//
	void RenderTarget::removeAllViewports()
	{
		mViewportList.clear();
	}
	//-------------------------------------------------------------//
	ushort RenderTarget::getNumViewports() const
	{
		return mViewportList.size();
	}
	//-------------------------------------------------------------//
	void RenderTarget::_beginUpdate()
	{

	}
	//-------------------------------------------------------------//
	void RenderTarget::_endUpdate()
	{

	}
	//-------------------------------------------------------------//
	void RenderTarget::_updateImpl()
	{
		_beginUpdate();
		_updateAutoUpdateViewports();
		_endUpdate();
	}
	//-------------------------------------------------------------//
	void RenderTarget::_updateAutoUpdateViewports()
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
	//-------------------------------------------------------------//
	void RenderTarget::_updateViewport(Viewport* vp)
	{
		vp->update();
	}
	//-------------------------------------------------------------//
	void RenderTarget::getCustomParams(const String& name, void* pDst)
	 {
		 TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR," you have called the empty base function", "RenderTarget::getCustomParams");
	 }
}