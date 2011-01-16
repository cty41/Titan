#ifndef __TITANCONFIG__HH
#define __TITANCONFIG__HH

#define TITAN_MEMORY_ALLOCATOR_STL 0
#define TITAN_MEMORY_ALLOCATOR_NED 1

//define MemoryAllocator
#define TITAN_MEMORY_ALLOCATOR 1		//use nedmalloc

//memory alignment
#define TITAN_SIMD_ALIGNMENT 16

#define  TITAN_MEMORY_TRACKER_MODE 0

//dll export
#ifdef TITAN_EXPORT
#	define _DllExport __declspec( dllexport )
#else
#	define _DllExport __declspec( dllimport )
#endif

#if _MSC_VER >= 1200
#	define	 FORCEINLINE __forceinline
#else
#	define	 FORCEINLINE __inline
#endif

#endif