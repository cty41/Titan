#include "TitanStableHeader.h"
#include "TiStringConverter.h"

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
	//------------------------------------------------------------------------------//
	String StringConverter::toString(float val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.precision(precision);
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//------------------------------------------------------------------------------//
	int StringConverter::parseInt(const String& val)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = 0;
		str >> ret;

		return ret;
	}
	//------------------------------------------------------------------------------//
	uint StringConverter::parseUint(const String& val)
	{
		StringStream str(val);
		uint ret = 0;
		str >> ret;

		return ret;
	}
	//------------------------------------------------------------------------------//
	float StringConverter::parseFloat(const String& val)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		float ret = 0;
		str >> ret;

		return ret;
	}
	
}