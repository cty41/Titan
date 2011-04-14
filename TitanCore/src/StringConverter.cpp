#include "TitanStableHeader.h"
#include "StringConverter.h"

namespace Titan
{
	String StringConverter::toString(size_t val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	int StringConverter::parseInt(const String& val)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = 0;
		str >> ret;

		return ret;
	}
}