#include "TitanStableHeader.h"
#include "Exception.h"
#include "ConsoleDebugger.h"


namespace Titan
{
	Exception::Exception(int number, const String& desc, const String& src)
		:line(0), number(number), description(desc), source(src)
	{
	}
	//-------------------------------------------------------------------------------//
	Exception::Exception(int number, const String& desc, const String& src, const char* type, const char* file, long line)
		:line(line), number(number), typeName(type), description(desc), source(src), file(file)
	{
		if(ConsoleDebugger::getSingletonPtr())
		{
			ConsoleDebugger::getSingleton().outputMessage(
					getFullDescription(),
					CML_CRITICAL);
		}
	}
	//-------------------------------------------------------------------------------//
	Exception::Exception(const Exception& rhs)
		: line( rhs.line ), 
		number( rhs.number ), 
		typeName( rhs.typeName ), 
		description( rhs.description ), 
		source( rhs.source ), 
		file( rhs.file )
	{
	}
	//-------------------------------------------------------------------------------//
	void Exception::operator = ( const Exception& rhs )
	{
		description = rhs.description;
		number = rhs.number;
		source = rhs.source;
		file = rhs.file;
		line = rhs.line;
		typeName = rhs.typeName;
	}
	//-------------------------------------------------------------------------------//
	const String& Exception::getFullDescription() const
	{
		if(fullDesc.empty())
		{
			StringStream desc;

			desc<<"Titan Exception(" << number << ":" << typeName << "): "
				<< description 
				<< " in " << source;

			if( line > 0)
				desc<<" at "<< file<<" (line "<< line<<")";

			fullDesc = desc.str();
		}

		return fullDesc;
	}

	int Exception::getNumber() const throw()
	{
		return number;
	}
}