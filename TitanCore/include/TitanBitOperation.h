//copied from ogre
#ifndef __TITAN_BITOPERATION_HH
#define __TITAN_BITOPERATION_HH

#include "TitanPrerequisites.h"

namespace Titan
{
	template<typename T>
	static FORCEINLINE bool isPO2(T n)
	{
		return (n & (n-1)) == 0;
	}
}

#endif