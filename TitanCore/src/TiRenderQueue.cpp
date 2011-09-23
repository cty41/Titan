#include "TitanStableHeader.h"
#include "TiRenderQueue.h"
#include "TiRenderQueueGroup.h"

namespace Titan
{
	RenderQueue::RenderQueue()
		:mDefaultGroup(NULL), mCurrentCam(NULL)
	{
		addRenderQueueGroup(RGT_Sky_Early);
		addRenderQueueGroup(RGT_Background);
		addRenderQueueGroup(RGT_SceneObject);
		addRenderQueueGroup(RGT_Sky_Late);
		addRenderQueueGroup(RGT_HUD);
		mDefaultGroup = getRenderQueueGroup(RGT_SceneObject);
		mDefaultGroup->setSortStrategy(SS_Shader_Tex_Dist);
	}
	//-------------------------------------------------------------------------------//
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
	//-------------------------------------------------------------------------------//
	void RenderQueue::clear()
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.begin(), itend = mRenderQueueGroupMap.end();
		while (it != itend)
		{
			it->second->clear();
			++it;
		}

	}
	//-------------------------------------------------------------------------------//
	void RenderQueue::addRenderable(Renderable* rend, uint type, ushort priority)
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.find(type);
		if(it == mRenderQueueGroupMap.end())
		{
			StringStream err;
			err<<type;
			TITAN_EXCEPT_ITEMLOST(
				"Render Queue Group with this type:"+ err.str() + "does not exist");
		}
		else
		{
			it->second->addRenderable(rend, priority);
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderQueue::addRenderable(Renderable* rend)
	{
		mDefaultGroup->addRenderable(rend, 0);
	}
	//-------------------------------------------------------------------------------//
	void RenderQueue::addRenderQueueGroup(uint type)
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.find(type);
		if (it == mRenderQueueGroupMap.end())
		{
			mRenderQueueGroupMap.insert(RenderQueueGroupMap::value_type(type, TITAN_NEW RenderQueueGroup(this,type))); 
		}
	}
	//-------------------------------------------------------------------------------//
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
	//-------------------------------------------------------------------------------//
	void RenderQueue::sort()
	{
		RenderQueueGroupMap::iterator it = mRenderQueueGroupMap.begin(), itend = mRenderQueueGroupMap.end();
		while (it != itend)
		{
			it->second->sort();
			++it;
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderQueue::setSortStrategy(SortStrategy ss)
	{
		mDefaultGroup->setSortStrategy(ss);
	}

}