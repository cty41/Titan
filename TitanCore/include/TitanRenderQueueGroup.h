#ifndef __TITAN_RENDERQUEUE_GROUP__HH
#define __TITAN_RENDERQUEUE_GROUP__HH

#include "TitanPrerequisites.h"
#include "TitanRenderQueue.h"
#include "TitanIteratorWrapper.h"

namespace Titan
{
	class _DllExport RenderQueueGroup : public GeneralAlloc
	{
	public:
		struct RenderQueueEntry
		{
			uint64	sortKey;
			Renderable*	renderable;

			RenderQueueEntry(Renderable* rend, uint64 key)
				:renderable(rend), sortKey(key)
			{}
		};

		struct _DllExport EntrySortMore
		{
			bool operator()(const RenderQueueEntry& a, const RenderQueueEntry& b) const
			{
				return a.sortKey > b.sortKey;
			}
		};

		struct _DllExport EntrySortLess
		{
			bool operator()(const RenderQueueEntry& a, const RenderQueueEntry& b) const
			{
				return a.sortKey < b.sortKey;
			}
		};
	public:
		typedef vector<RenderQueueEntry>::type RenderQueueEntryVec;
		typedef VectorIterator<RenderQueueEntryVec> RenderQueueEntryVecIterator;

	public:
		RenderQueueGroup(RenderQueue* parent, uint type);

		~RenderQueueGroup();

		void addRenderable(Renderable* rend, ushort priority);

		void clear();

		void sort();

		void setSortStrategy(RenderQueue::SortStrategy ss){ mStrategy = ss;}

		RenderQueueEntryVecIterator getOpaqueEntryIterator() { return RenderQueueEntryVecIterator(mOpaqueEntryVec.begin(), mOpaqueEntryVec.end());}

		RenderQueueEntryVecIterator getTransparentEntryIterator() { return RenderQueueEntryVecIterator(mTransparentEntryVec.begin(), mTransparentEntryVec.end());}

	protected:
		RenderQueue*			mParentQueue;
		RenderQueueEntryVec		mOpaqueEntryVec;
		RenderQueueEntryVec		mTransparentEntryVec;
		uint					mType;
		uint					mStrategy;
	};
}

#endif