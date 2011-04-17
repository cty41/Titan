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

		void createSingleStripGrid(
			uint16 xVerts,	// width of grid
			uint16 yVerts,	// height of grid
			uint16 xStep,	// horz vertex count per cell
			uint16 yStep,	// vert vertex count per cell
			uint16 stride,	// horz vertex count in vbuffer
			uint16 flags);

	protected:
		HardwareBufferMgr*	mMgr;
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