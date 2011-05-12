#ifndef _TITIAN_INDEXBUFFER_HH
#define _TITIAN_INDEXBUFFER_HH

#include "TiPrerequisites.h"
#include "TiHardwareBuffer.h"
#include "TiSharedPtr.h"

namespace Titan
{
	class _DllExport IndexBuffer : public HardwareBuffer
	{
	public:
		IndexBuffer(HardwareBufferMgr* mgr, size_t numIndexes, HardwareBuffer::Usage usage,
			bool useSystemMemory);
		virtual ~IndexBuffer();

		size_t getNumIndexes(void) const { return mNumIndexes; }
		/// Get the size in bytes of each index
		size_t getIndexSize(void) const { return mIndexSize; }

		void createSingleStripGrid(uint16 xVerts, uint16 yVerts, uint16 xStep, uint16 yStep, uint16 stride);	

		void createTriangleListGrid(uint16 xVerts, uint16 yVerts, uint16 xStep, uint16 yStep, uint16 stride);

	protected:
		HardwareBufferMgr*	mMgr;
		size_t mNumIndexes;
		size_t mIndexSize;
	};

	class _DllExport IndexBufferPtr : public SharedPtr<IndexBuffer>
	{
	public:
		IndexBufferPtr() : SharedPtr<IndexBuffer>() {}
		explicit IndexBufferPtr(IndexBuffer* buf);

	};
}
#endif