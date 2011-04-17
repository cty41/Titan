#ifndef __TITAN_RENDERQUEUE_GROUP__HH
#define __TITAN_RENDERQUEUE_GROUP__HH

#include "TiPrerequisites.h"
#include "TiRenderQueue.h"
#include "TiIteratorWrapper.h"

namespace Titan
{
	class _DllExport RenderQueueGroup : public GeneralAlloc
	{
	public:
		struct RenderQueueEntry
		{
			uint64	sortKey;
			Renderable*	renderable;
			Pass*		rendPass;

			RenderQueueEntry(Renderable* rend, Pass* pass, uint64 key)
				:renderable(rend),rendPass(pass), sortKey(key)
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

		RenderQueueEntryVecIterator getSortedTransparentEntryIterator() { return RenderQueueEntryVecIterator(mSortedTransparentEntryVec.begin(), mSortedTransparentEntryVec.end());}

		RenderQueueEntryVecIterator getUnsortedTransparentEntryIterator() { return RenderQueueEntryVecIterator(mUnsortedTransparentEntryVec.begin(), mUnsortedTransparentEntryVec.end());}

	protected:
		RenderQueue*			mParentQueue;
		RenderQueueEntryVec		mOpaqueEntryVec;
		RenderQueueEntryVec		mSortedTransparentEntryVec;
		RenderQueueEntryVec		mUnsortedTransparentEntryVec;
		uint					mType;
		uint					mStrategy;
	};
}

#endif