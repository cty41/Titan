#include "TitanStableHeader.h"
#include "TitanRenderQueue.h"
#include "TitanRenderQueueGroup.h"

namespace Titan
{
	RenderQueue::RenderQueue()
		:mDefaultGroup(NULL), mCurrentCam(NULL)
	{
		addRenderQueueGroup(RGT_Background);
		addRenderQueueGroup(RGT_SceneObject);
		addRenderQueueGroup(RGT_HUD);
		mDefaultGroup = getRenderQueueGroup(RGT_SceneObject);
		mDefaultGroup->setSortStrategy(SS_Shader_Tex_Dist);
	}
	//-------------------------------------------------------------//
	RenderQueue::~RenderQueue()
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.begin(), itend = mRenderQueueGroupMap.end();
		while (it != itend)
		{
			it->second->clear();
			TITAN_DELETE (it->second);
			++it;
		}
		mRenderQueueGroupMap.clear();
	}
	//-------------------------------------------------------------//
	void RenderQueue::clear()
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.begin(), itend = mRenderQueueGroupMap.end();
		while (it != itend)
		{
			it->second->clear();
			++it;
		}

	}
	//-------------------------------------------------------------//
	void RenderQueue::addRenderable(Renderable* rend)
	{
		mDefaultGroup->addRenderable(rend, RGT_SceneObject);
	}
	//-------------------------------------------------------------//
	void RenderQueue::addRenderQueueGroup(uint type)
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.find(type);
		if (it == mRenderQueueGroupMap.end())
		{
			mRenderQueueGroupMap.insert(RenderQueueGroupMap::value_type(type, TITAN_NEW RenderQueueGroup(this,type))); 
		}
	}
	//-------------------------------------------------------------//
	RenderQueueGroup*	RenderQueue::getRenderQueueGroup(uint type)
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.find(type);
		if (it == mRenderQueueGroupMap.end())
		{
			RenderQueueGroup* rqg = TITAN_NEW RenderQueueGroup(this, type);
			mRenderQueueGroupMap.insert(RenderQueueGroupMap::value_type(type, rqg)); 
			return rqg;
		}
		else
			return it->second;

	}
	//-------------------------------------------------------------//
	void RenderQueue::sort()
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.begin(), itend = mRenderQueueGroupMap.end();
		while (it != itend)
		{
			it->second->sort();
			++it;
		}
	}
	//-------------------------------------------------------------//
	void RenderQueue::setSortStrategy(SortStrategy ss)
	{
		mDefaultGroup->setSortStrategy(ss);
	}

}