#ifndef __TITAN_MEMORYCONFIG_HH
#define __TITAN_MEMORYCONFIG_HH

#include "TiMemoryAllocatedObject.h"

namespace Titan
{
	enum MemoryCategory
	{
		/// General purpose, add later
		MEMCATEGORY_GENERAL = 0
	};
}

#include "TiMemoryAllocatedObject.h"
#include "TiMemorySTLAllocator.h"

#if TITAN_MEMORY_ALLOCATOR == TITAN_MEMORY_ALLOCATOR_NED

#include "TiMemoryNedAlloc.h"
namespace Titan
{
	template <MemoryCategory Cat> class CategorisedAllocPolicy : public NedAllocPolicy{};
	template <MemoryCategory Cat, size_t align = 0> class CategorisedAlignAllocPolicy : public NedAlignedAllocPolicy<align>{};
}
#elif TITAN_MEMORY_ALLOCATOR == TITAN_MEMORY_ALLOCATOR_STD

#  include "TiMemoryStdAlloc.h"
namespace Titan
{
	template <MemoryCategory Cat> class CategorisedAllocPolicy : public StdAllocPolicy{};
	template <MemoryCategory Cat, size_t align = 0> class CategorisedAlignAllocPolicy : public StdAlignedAllocPolicy<align>{};
}
#endif

namespace Titan
{
	typedef CategorisedAllocPolicy<MEMCATEGORY_GENERAL> GeneralAllocPolicy;


	typedef AllocatedObject<GeneralAllocPolicy> GeneralAllocatedObject;

	typedef GeneralAllocatedObject				GeneralAlloc;
	//add new allocate policy later, very far future~
}

namespace Titan
{
	/** Utility function for constructing an array of objects with placement new,
		without using new[] (which allocates an undocumented amount of extra memory
		and so isn't appropriate for custom allocators).
	*/
	template<typename T>
	T* constructN(T* basePtr, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			new ((void*)(basePtr+i)) T;
		}
		return basePtr;
	}
}

#if _DEBUG

/// Allocate a block of raw memory, and indicate the category of usage
#	define TITAN_MALLOC(bytes, category) ::Titan::CategorisedAllocPolicy<category>::allocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of memory for a primitive type, and indicate the category of usage
#	define TITAN_ALLOC_T(T, count, category) static_cast<T*>(::Titan::CategorisedAllocPolicy<category>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Free the memory allocated with TITAN_MALLOC or TITAN_ALLOC_T. Category is required to be restated to ensure the matching policy is used
#	define TITAN_FREE(ptr, category) ::Titan::CategorisedAllocPolicy<category>::deallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type with constructor parameters
#	define TITAN_NEW_T(T, category) new (::Titan::CategorisedAllocPolicy<category>::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types - do not use for classes that inherit from AllocatedObject
#	define TITAN_NEW_ARRAY_T(T, count, category) ::Titan::constructN(static_cast<T*>(::Titan::CategorisedAllocPolicy<category>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count) 
/// Free the memory allocated with TITAN_NEW_T. Category is required to be restated to ensure the matching policy is used
#	define TITAN_DELETE_T(ptr, T, category) if(ptr){(ptr)->~T(); ::Titan::CategorisedAllocPolicy<category>::deallocateBytes((void*)ptr);}
/// Free the memory allocated with TITAN_NEW_ARRAY_T. Category is required to be restated to ensure the matching policy is used, count and type to call destructor
#	define TITAN_DELETE_ARRAY_T(ptr, T, count, category) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::Titan::CategorisedAllocPolicy<category>::deallocateBytes((void*)ptr);}

// aligned allocation
/// Allocate a block of raw memory aligned to SIMD boundaries, and indicate the category of usage
#	define TITAN_MALLOC_SIMD(bytes, category) ::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of raw memory aligned to user defined boundaries, and indicate the category of usage
#	define TITAN_MALLOC_ALIGN(bytes, category, align) ::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of memory for a primitive type aligned to SIMD boundaries, and indicate the category of usage
#	define TITAN_ALLOC_T_SIMD(T, count, category) static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Allocate a block of memory for a primitive type aligned to user defined boundaries, and indicate the category of usage
#	define TITAN_ALLOC_T_ALIGN(T, count, category, align) static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Free the memory allocated with either TITAN_MALLOC_SIMD or TITAN_ALLOC_T_SIMD. Category is required to be restated to ensure the matching policy is used
#	define TITAN_FREE_SIMD(ptr, category) ::Titan::CategorisedAlignAllocPolicy<category>::deallocateBytes(ptr)
/// Free the memory allocated with either TITAN_MALLOC_ALIGN or TITAN_ALLOC_T_ALIGN. Category is required to be restated to ensure the matching policy is used
#	define TITAN_FREE_ALIGN(ptr, category, align) ::Titan::CategorisedAlignAllocPolicy<category, align>::deallocateBytes(ptr)

/// Allocate space for one primitive type, external type or non-virtual type aligned to SIMD boundaries
#	define TITAN_NEW_T_SIMD(T, category) new (::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types aligned to SIMD boundaries - do not use for classes that inherit from AllocatedObject
#	define TITAN_NEW_ARRAY_T_SIMD(T, count, category) ::Titan::constructN(static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count) 
/// Free the memory allocated with TITAN_NEW_T_SIMD. Category is required to be restated to ensure the matching policy is used
#	define TITAN_DELETE_T_SIMD(ptr, T, category) if(ptr){(ptr)->~T(); ::Titan::CategorisedAlignAllocPolicy<category>::deallocateBytes(ptr);}
/// Free the memory allocated with TITAN_NEW_ARRAY_T_SIMD. Category is required to be restated to ensure the matching policy is used, count and type to call destructor
#	define TITAN_DELETE_ARRAY_T_SIMD(ptr, T, count, category) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::Titan::CategorisedAlignAllocPolicy<category>::deallocateBytes(ptr);}
/// Allocate space for one primitive type, external type or non-virtual type aligned to user defined boundaries
#	define TITAN_NEW_T_ALIGN(T, category, align) new (::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types aligned to user defined boundaries - do not use for classes that inherit from AllocatedObject
#	define TITAN_NEW_ARRAY_T_ALIGN(T, count, category, align) ::Titan::constructN(static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count) 
/// Free the memory allocated with TITAN_NEW_T_ALIGN. Category is required to be restated to ensure the matching policy is used
#	define TITAN_DELETE_T_ALIGN(ptr, T, category, align) if(ptr){(ptr)->~T(); ::Titan::CategorisedAlignAllocPolicy<category, align>::deallocateBytes(ptr);}
/// Free the memory allocated with TITAN_NEW_ARRAY_T_ALIGN. Category is required to be restated to ensure the matching policy is used, count and type to call destructor
#	define TITAN_DELETE_ARRAY_T_ALIGN(ptr, T, count, category, align) if(ptr){for (size_t _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} ::Titan::CategorisedAlignAllocPolicy<category, align>::deallocateBytes(ptr);}

// new / delete for classes deriving from AllocatedObject (alignment determined by per-class policy)
// Also hooks up the file/line/function params
// Can only be used with classes that derive from AllocatedObject since customised new/delete needed
#	define TITAN_NEW new (__FILE__, __LINE__, __FUNCTION__)
#	define TITAN_DELETE delete

#define TITAN_SAFE_DELETE(p) if(p) { TITAN_DELETE p; p = nullptr;}



#else // !TITAN_DEBUG_MODE

/// Allocate a block of raw memory, and indicate the category of usage
#	define TITAN_MALLOC(bytes, category) ::Titan::CategorisedAllocPolicy<category>::allocateBytes(bytes)
/// Allocate a block of memory for a primitive type, and indicate the category of usage
#	define TITAN_ALLOC_T(T, count, category) static_cast<T*>(::Titan::CategorisedAllocPolicy<category>::allocateBytes(sizeof(T)*(count)))
/// Free the memory allocated with TITAN_MALLOC or TITAN_ALLOC_T. Category is required to be restated to ensure the matching policy is used
#	define TITAN_FREE(ptr, category) ::Titan::CategorisedAllocPolicy<category>::deallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type with constructor parameters
#	define TITAN_NEW_T(T, category) new (::Titan::CategorisedAllocPolicy<category>::allocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types - do not use for classes that inherit from AllocatedObject
#	define TITAN_NEW_ARRAY_T(T, count, category) ::Titan::constructN(static_cast<T*>(::Titan::CategorisedAllocPolicy<category>::allocateBytes(sizeof(T)*(count))), count) 
/// Free the memory allocated with TITAN_NEW_T. Category is required to be restated to ensure the matching policy is used
#	define TITAN_DELETE_T(ptr, T, category) if(ptr){(ptr)->~T(); ::Titan::CategorisedAllocPolicy<category>::deallocateBytes((void*)ptr);}
/// Free the memory allocated with TITAN_NEW_ARRAY_T. Category is required to be restated to ensure the matching policy is used, count and type to call destructor
#	define TITAN_DELETE_ARRAY_T(ptr, T, count, category) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::Titan::CategorisedAllocPolicy<category>::deallocateBytes((void*)ptr);}

// aligned allocation
/// Allocate a block of raw memory aligned to SIMD boundaries, and indicate the category of usage
#	define TITAN_MALLOC_SIMD(bytes, category) ::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(bytes)
/// Allocate a block of raw memory aligned to user defined boundaries, and indicate the category of usage
#	define TITAN_MALLOC_ALIGN(bytes, category, align) ::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(bytes)
/// Allocate a block of memory for a primitive type aligned to SIMD boundaries, and indicate the category of usage
#	define TITAN_ALLOC_T_SIMD(T, count, category) static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(sizeof(T)*(count)))
/// Allocate a block of memory for a primitive type aligned to user defined boundaries, and indicate the category of usage
#	define TITAN_ALLOC_T_ALIGN(T, count, category, align) static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(sizeof(T)*(count)))
/// Free the memory allocated with either TITAN_MALLOC_SIMD or TITAN_ALLOC_T_SIMD. Category is required to be restated to ensure the matching policy is used
#	define TITAN_FREE_SIMD(ptr, category) ::Titan::CategorisedAlignAllocPolicy<category>::deallocateBytes((void*)ptr)
/// Free the memory allocated with either TITAN_MALLOC_ALIGN or TITAN_ALLOC_T_ALIGN. Category is required to be restated to ensure the matching policy is used
#	define TITAN_FREE_ALIGN(ptr, category, align) ::Titan::CategorisedAlignAllocPolicy<category, align>::deallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type aligned to SIMD boundaries
#	define TITAN_NEW_T_SIMD(T, category) new (::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types aligned to SIMD boundaries - do not use for classes that inherit from AllocatedObject
#	define TITAN_NEW_ARRAY_T_SIMD(T, count, category) ::Titan::constructN(static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category>::allocateBytes(sizeof(T)*(count))), count) 
/// Free the memory allocated with TITAN_NEW_T_SIMD. Category is required to be restated to ensure the matching policy is used
#	define TITAN_DELETE_T_SIMD(ptr, T, category) if(ptr){(ptr)->~T(); ::Titan::CategorisedAlignAllocPolicy<category>::deallocateBytes((void*)ptr);}
/// Free the memory allocated with TITAN_NEW_ARRAY_T_SIMD. Category is required to be restated to ensure the matching policy is used, count and type to call destructor
#	define TITAN_DELETE_ARRAY_T_SIMD(ptr, T, count, category) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::Titan::CategorisedAlignAllocPolicy<category>::deallocateBytes((void*)ptr);}
/// Allocate space for one primitive type, external type or non-virtual type aligned to user defined boundaries
#	define TITAN_NEW_T_ALIGN(T, category, align) new (::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types aligned to user defined boundaries - do not use for classes that inherit from AllocatedObject
#	define TITAN_NEW_ARRAY_T_ALIGN(T, count, category, align) ::Titan::constructN(static_cast<T*>(::Titan::CategorisedAlignAllocPolicy<category, align>::allocateBytes(sizeof(T)*(count))), count) 
/// Free the memory allocated with TITAN_NEW_T_ALIGN. Category is required to be restated to ensure the matching policy is used
#	define TITAN_DELETE_T_ALIGN(ptr, T, category, align) if(ptr){(ptr)->~T(); ::Titan::CategorisedAlignAllocPolicy<category, align>::deallocateBytes((void*)ptr);}
/// Free the memory allocated with TITAN_NEW_ARRAY_T_ALIGN. Category is required to be restated to ensure the matching policy is used, count and type to call destructor
#	define TITAN_DELETE_ARRAY_T_ALIGN(ptr, T, count, category, align) if(ptr){for (size_t _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} ::Titan::CategorisedAlignAllocPolicy<category, align>::deallocateBytes((void*)ptr);}

// new / delete for classes deriving from AllocatedObject (alignment determined by per-class policy)
#	define TITAN_NEW new 
#	define TITAN_DELETE delete

#define TITAN_SAFE_DELETE(p) if(p) { TITAN_DELETE p; p = nullptr;}

#endif // TITAN_DEBUG_MODE


namespace Titan
{
	/** Function which invokes TITAN_DELETE on a given pointer. 
	@remarks
		Useful to pass custom deletion policies to external libraries (e. g. boost).
	*/
	template<typename T>
	void deletePtr(T* ptr)
	{
		TITAN_DELETE ptr;
	}
}


#endif