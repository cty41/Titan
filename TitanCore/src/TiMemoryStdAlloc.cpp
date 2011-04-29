#include "TitanStableHeader.h"
#include "TiMemoryStdAlloc.h"
#include "TiBitOperation.h"

/**
*
* |___2___|3|_________5__________|__6__|
* ^         ^
* 1         4
*
* 1 -> Pointer to start of the block allocated by new.
* 2 -> Gap used to get 4 aligned on given alignment
* 3 -> Byte offset between 1 and 4
* 4 -> Pointer to the start of data block.
* 5 -> Data block.
* 6 -> Wasted memory at rear of data block.
*/
#if TITAN_MEMORY_ALLOCATOR == TITAN_MEMORY_ALLOCATOR_STD

namespace Titan 
{

#define TITAN_SIMD_ALIGNMENT 16
    //---------------------------------------------------------------------
    void* AlignedMemory::allocate(size_t size, size_t alignment)
    {
        assert(0 < alignment && alignment <= 128 && isPO2(alignment));

        unsigned char* p = new unsigned char[size + alignment];
        size_t offset = alignment - (size_t(p) & (alignment-1));

        unsigned char* result = p + offset;
        result[-1] = (unsigned char)offset;

        return result;
    }
    //---------------------------------------------------------------------
    void* AlignedMemory::allocate(size_t size)
    {
        return allocate(size, TITAN_SIMD_ALIGNMENT);
    }
    //---------------------------------------------------------------------
    void AlignedMemory::deallocate(void* p)
    {
        if (p)
        {
            unsigned char* mem = (unsigned char*)p;
            mem = mem - mem[-1];
            delete [] mem;
        }
    }

}
#endif