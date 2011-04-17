#ifndef __TITAN_CONVERTER__HH
#define __TITAN_CONVERTER__HH

#include "TiPrerequisites.h"

namespace Titan
{
	class _DllExport StringConverter
	{
	public:

		static String toString(size_t val, 
			unsigned short width = 0, char fill = ' ', 
			std::ios::fmtflags flags = std::ios::fmtflags(0) );

		static int parseInt(const String& val);

	};
}
#endif