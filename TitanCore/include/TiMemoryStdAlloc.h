#ifndef __TITAN_MEMORY_STD_ALLOC__HH
#define __TITAN_MEMORY_STD_ALLOC__HH

#include <memory>
#include <limits>

#include "TiMemoryTracker.h"

#if TITAN_MEMORY_ALLOCATOR == TITAN_MEMORY_ALLOCATOR_STD
namespace Titan
{


	class _DllExport AlignedMemory
	{
	public:
        static void* allocate(size_t size, size_t alignment);

        static void* allocate(size_t size);

        static void deallocate(void* p);
	};

	class _DllExport StdAllocPolicy
	{
	public:
		static inline void* allocateBytes(size_t count, 
#if TITAN_MEMORY_TRACKER
			const char* file = 0, int line = 0, const char* func = 0
#else
			const char*  = 0, int  = 0, const char* = 0
#endif
            )
		{
			void* ptr = malloc(count);
#if TITAN_MEMORY_TRACKER
			// this alloc policy doesn't do pools
			MemoryTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#endif
			return ptr;
		}

		static inline void deallocateBytes(void* ptr)
		{
#if TITAN_MEMORY_TRACKER
			MemoryTracker::get()._recordDealloc(ptr);
#endif
			free(ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// no instantiation
		StdAllocPolicy()
		{ }
	};

	template <size_t Alignment = 0>
	class StdAlignedAllocPolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment
			[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, 
#if TITAN_MEMORY_TRACKER
			const char* file = 0, int line = 0, const char* func = 0
#else
			const char*  = 0, int  = 0, const char* = 0
#endif
            )
		{
			void* ptr = Alignment ? AlignedMemory::allocate(count, Alignment)
				: AlignedMemory::allocate(count);
#if TITAN_MEMORY_TRACKER
			// this alloc policy doesn't do pools
			MemoryTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#endif
			return ptr;
		}

		static inline void deallocateBytes(void* ptr)
		{
#if TITAN_MEMORY_TRACKER
			MemoryTracker::get()._recordDealloc(ptr);
#endif
			AlignedMemory::deallocate(ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// No instantiation
		StdAlignedAllocPolicy()
		{ }
	};


}
#endif

#endif // __MemoryStdAlloc_H__
