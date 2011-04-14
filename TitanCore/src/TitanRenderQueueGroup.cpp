#include "TitanStableHeader.h"
#include "TitanRenderQueueGroup.h"
#include "TiRenderable.h"
#include "TitanRenderQueue.h"
#include "TiPass.h"
#include <algorithm>

namespace Titan
{
	RenderQueueGroup::RenderQueueGroup(RenderQueue* parent,uint type)
		:mParentQueue(parent), mStrategy(0), mType(type)
	{

	}
	//-------------------------------------------------------------------------------//
	RenderQueueGroup::~RenderQueueGroup()
	{

	}
	//-------------------------------------------------------------------------------//
	void RenderQueueGroup::addRenderable(Renderable* rend, ushort priority)
	{
		uint64 sortKey = 0;
		if(mStrategy == RenderQueue::SS_No_Sort)
		{
			//which means we only sort by priority
			Material::PassVecIterator pit = rend->getMaterial()->getPassIterator();
			while (pit.hasMoreElements())
			{
				sortKey = priority;
				RenderQueueEntry entry = RenderQueueEntry(rend, pit.peekNext(), sortKey);
				if(!rend->getMaterial()->isTransparent())
				{
					mOpaqueEntryVec.push_back(entry);
				}
				else
				{
					if(rend->getMaterial()->isTransparentSorted())
						mSortedTransparentEntryVec.push_back(entry);
					else
						mUnsortedTransparentEntryVec.push_back(entry);
				}
				pit.next();
			}

		}
		else if (mStrategy == RenderQueue::SS_Shader_Tex_Dist)
		{
			Material::PassVecIterator pit = rend->getMaterial()->getPassIterator();
			while (pit.hasMoreElements())
			{
				//priority
				uint64 pri = priority;
				pri <<= 56;
				pri &= 0xFF00000000000000;
				sortKey |=  pri;
				//is opaque?
				uint64 ShaderId = rend->getShaderEffect()->getID();
				ShaderId = ShaderId << 45;
				ShaderId &= 0x00FFE00000000000;
				sortKey |= ShaderId;
				//material id

				//distance
				uint64 dist = (uint64)rend->getSquaredDistance(mParentQueue->getCurrentCam());
				dist &= 0x00000000FFFFFFFF;
				dist = 0x00000000FFFFFFFF - dist;
				sortKey |= dist;
				RenderQueueEntry entry = RenderQueueEntry(rend, pit.peekNext(), sortKey);

				if(!rend->getMaterial()->isTransparent())
				{
					mOpaqueEntryVec.push_back(entry);
				}
				else
				{
					if(rend->getMaterial()->isTransparentSorted())
						mSortedTransparentEntryVec.push_back(entry);
					else
						mUnsortedTransparentEntryVec.push_back(entry);
				}
				pit.next();
			}	
			
		}
	}
	//-------------------------------------------------------------------------------//
	void RenderQueueGroup::clear()
	{
		mOpaqueEntryVec.clear();
		mSortedTransparentEntryVec.clear();
		mUnsortedTransparentEntryVec.clear();
	}
	//-------------------------------------------------------------------------------//
	void RenderQueueGroup::sort()
	{
		if(!mSortedTransparentEntryVec.empty())
			std::sort(mSortedTransparentEntryVec.begin(), mSortedTransparentEntryVec.end(), EntrySortMore());
		if(!mUnsortedTransparentEntryVec.empty())
			std::sort(mUnsortedTransparentEntryVec.begin(), mUnsortedTransparentEntryVec.end(), EntrySortMore());
		if(!mOpaqueEntryVec.empty())
			std::sort(mOpaqueEntryVec.begin(), mOpaqueEntryVec.end(), EntrySortMore());
	}
	//-------------------------------------------------------------------------------//

}