#include "TitanStableHeader.h"
#include "TitanRenderQueueGroup.h"
#include "Renderable.h"
#include "TitanRenderQueue.h"
#include <algorithm>

namespace Titan
{
	RenderQueueGroup::RenderQueueGroup(RenderQueue* parent,uint type)
		:mParentQueue(parent), mStrategy(0), mType(type)
	{

	}
	//-------------------------------------------------------------//
	RenderQueueGroup::~RenderQueueGroup()
	{

	}
	//-------------------------------------------------------------//
	void RenderQueueGroup::addRenderable(Renderable* rend, ushort priority)
	{
		uint64 sortKey = 0;
		if(mStrategy == RenderQueue::SS_No_Sort)
		{
			//which means we only sort by priority
			sortKey = priority;
		}
		else if (mStrategy == RenderQueue::SS_Shader_Tex_Dist)
		{
			//priority
			uint64 pri = priority;
			pri <<= 56;
			pri &= 0xFF00000000000000;
			sortKey |=  pri;
			//is opaque?
			if(true)
			{
				uint64 ShaderId = rend->getShaderEffect()->getID();
				ShaderId = ShaderId << 45;
				ShaderId &= 0x00FFE00000000000;
				sortKey |= ShaderId;
				//material id

				//distance
				assert(mParentQueue->getCurrentCam() != NULL);
				uint64 dist = (uint64)rend->getSquaredDistance(mParentQueue->getCurrentCam());
				dist &= 0x00000000FFFFFFFF;
				dist = 0x00000000FFFFFFFF - dist;
				sortKey |= dist;
				RenderQueueEntry entry = RenderQueueEntry(rend, sortKey);
				mOpaqueEntryVec.push_back(entry);
			}
			else
			{

			}
		}
	}
	//-------------------------------------------------------------//
	void RenderQueueGroup::clear()
	{
		mOpaqueEntryVec.clear();
		mTransparentEntryVec.clear();
	}
	//-------------------------------------------------------------//
	void RenderQueueGroup::sort()
	{
		std::sort(mTransparentEntryVec.begin(), mTransparentEntryVec.end(), EntrySortMore());
		std::sort(mOpaqueEntryVec.begin(), mOpaqueEntryVec.end(), EntrySortMore());
	}
	//-------------------------------------------------------------//

}