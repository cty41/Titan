#include "TitanStableHeader.h"
#include "StringConverter.h"

namespace Titan
{
	int StringConverter::parseInt(const String& val)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = 0;
		str >> ret;

		return ret;
	}
}