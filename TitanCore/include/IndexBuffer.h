#ifndef _TITIAN_INDEXBUFFER_HH
#define _TITIAN_INDEXBUFFER_HH

#include "TitanPrerequisites.h"
#include "HardwareBuffer.h"
#include "TitanSharedPtr.h"

namespace Titan
{
	class _DllExport IndexBuffer : public HardwareBuffer
	{
	public:
		IndexBuffer(HardwareBufferManager* mgr, size_t numIndexes, HardwareBuffer::Usage usage,
			bool useSystemMemory);
		virtual ~IndexBuffer();

		size_t getNumIndexes(void) const { return mNumIndexes; }
		/// Get the size in bytes of each index
		size_t getIndexSize(void) const { return mIndexSize; }

	protected:
		HardwareBufferManager*	mMgr;
		size_t mNumIndexes;
		size_t mIndexSize;
	};

	class _DllExport IndexBufferSharedPtr : public SharedPtr<IndexBuffer>
	{
	public:
		IndexBufferSharedPtr() : SharedPtr<IndexBuffer>() {}
		explicit IndexBufferSharedPtr(IndexBuffer* buf);

	};
}
#endif