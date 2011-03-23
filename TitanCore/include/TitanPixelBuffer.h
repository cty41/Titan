#ifndef __TITAN_PIXELVUFFER__HH
#define __TITAN_PIXELVUFFER__HH

#include "TitanPrerequisites.h"
#include "HardwareBuffer.h"
#include "TitanCommon.h"
#include "TitanPixelFormat.h"

namespace Titan
{
	class _DllExport PixelBuffer : public HardwareBuffer
	{
	public:
		PixelBuffer(size_t width, size_t height, size_t depth,
			PixelFormat format, HardwareBuffer::Usage usage, bool useSystemMemory);

		~PixelBuffer();

		using HardwareBuffer::lock;

		virtual const PixelBox& lock(const Box& lockBox, LockOptions options);

		virtual void* lock(size_t offset, size_t length, LockOptions options);

		virtual void readData(size_t offset, size_t length, void* pDest);

		virtual void writeData(size_t offset, size_t length, const void* pSource,
			bool discardWholeBuffer = false);


		const PixelBox& getCurrentLock();

	protected:
		// Extents
		size_t mWidth, mHeight, mDepth;
		// Pitches (offsets between rows and slices)
		size_t mRowPitch, mSlicePitch;
		// Internal format
		PixelFormat mFormat;
		// Currently locked region (local coords)
		PixelBox mCurrentLock;
		// The current locked box of this surface (entire surface coords)
		Box mLockedBox;
	};
}

#endif