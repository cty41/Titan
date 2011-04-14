//copied from ogre
#ifndef __TITAN_BITOPERATION_HH
#define __TITAN_BITOPERATION_HH

#include "TitanPrerequisites.h"

namespace Titan
{
	static FORCEINLINE uint32 firstPO2From(uint32 n)
	{
		--n;            
		n |= n >> 16;
		n |= n >> 8;
		n |= n >> 4;
		n |= n >> 2;
		n |= n >> 1;
		++n;
		return n;
	}
	
	template<typename T>
	static FORCEINLINE bool isPO2(T n)
	{
		return (n & (n-1)) == 0;
	}
}

#endif