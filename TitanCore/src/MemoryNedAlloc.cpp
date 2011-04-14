#include "TitanStableHeader.h"
#include "MemoryNedAlloc.h"
#include "MemoryTracker.h"

#if TITAN_MEMORY_ALLOCATOR == TITAN_MEMORY_ALLOCATOR_NED

#define ABORT_ON_ASSERT_FAILURE 0
#include "nedmalloc.c"

namespace Titan
{
	//-------------------------------------------------------------------------------//
	void* NedAllocImpl::allocBytes(size_t count, 
		const char* file, int line, const char* func)
	{
		void* ptr = nedalloc::nedmalloc(count);
#if TITAN_MEMORY_TRACKER
		// this alloc policy doesn't do pools (yet, ned can do it)
		MemoryTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#endif
		return ptr;
	}
	//-------------------------------------------------------------------------------//
	void NedAllocImpl::deallocBytes(void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
#if TITAN_MEMORY_TRACKER
		MemoryTracker::get()._recordDealloc(ptr);
#endif
		nedalloc::nedfree(ptr);
	}
	//-------------------------------------------------------------------------------//
	void* NedAllocImpl::allocBytesAligned(size_t align, size_t count, 
		const char* file, int line, const char* func)
	{
		// default to platform SIMD alignment if none specified
		void* ptr =  align ? nedalloc::nedmemalign(align, count)
			: nedalloc::nedmemalign(TITAN_SIMD_ALIGNMENT, count);
#if TITAN_MEMORY_TRACKER
		// this alloc policy doesn't do pools (yet, ned can do it)
		MemoryTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#endif
		return ptr;
	}
	//-------------------------------------------------------------------------------//
	void NedAllocImpl::deallocBytesAligned(size_t align, void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
#if TITAN_MEMORY_TRACKER
		// this alloc policy doesn't do pools (yet, ned can do it)
		MemoryTracker::get()._recordDealloc(ptr);
#endif
		nedalloc::nedfree(ptr);
	}

}

#endif