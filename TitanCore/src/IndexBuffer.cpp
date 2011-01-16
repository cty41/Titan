#include "TitanStableHeader.h"
#include "IndexBuffer.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	IndexBuffer::IndexBuffer(HardwareBufferManager* mgr, size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory)
		: HardwareBuffer(usage, useSystemMemory), mMgr(mgr), mNumIndexes(numIndexes)
	{
		//change in the future
		mIndexSize = 16;
		mSizeInBytes = mIndexSize * mNumIndexes;
	}
	//-------------------------------------------------------------//
	IndexBuffer::~IndexBuffer()
	{
		mMgr->_notifyIndexBufferDestroyed(this);
	}

	//-------------------------------------------------------------//
	IndexBufferSharedPtr::IndexBufferSharedPtr(IndexBuffer* buf)
		: SharedPtr<IndexBuffer>(buf)
	{
	}
}