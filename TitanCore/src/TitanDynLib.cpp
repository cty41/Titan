#include "TitanStableHeader.h"
#include "TitanDynLib.h"
#include "Exception.h"
#include "ConsoleDebugger.h"

#if WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#endif

namespace Titan
{
	DynLib::DynLib( const String& name )
	{
		mName = name;
		m_hInst = NULL;
	}

	//-------------------------------------------------------------------------------//--
	DynLib::~DynLib()
	{
	}

	//-------------------------------------------------------------------------------//--
	void DynLib::load()
	{
		// Log library load
		ConsoleDebugger::getSingleton().stream()<<"Loading library " << mName;

		String name = mName;

#if WIN32
		// Although LoadLibraryEx will add .dll itself when you only specify the library name,
		// if you include a relative path then it does not. So, add it to be sure.
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";
#endif
		m_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD( name.c_str() );

		if( !m_hInst )
			TITAN_EXCEPT(
			Exception::EXCEP_INTERNAL_ERROR, 
			"Could not load dynamic library " + mName + 
			".  System Error: " + dynlibError(),
			"DynLib::load" );
	}

	//-------------------------------------------------------------------------------//--
	void DynLib::unload()
	{
		// Log library unload
		ConsoleDebugger::getSingleton().stream() << "Unloading library " << mName;

		if( DYNLIB_UNLOAD( m_hInst ) )
		{
			TITAN_EXCEPT(
				Exception::EXCEP_INTERNAL_ERROR, 
				"Could not unload dynamic library " + mName +
				".  System Error: " + dynlibError(),
				"DynLib::unload");
		}

	}

	//-------------------------------------------------------------------------------//--
	void* DynLib::getSymbol( const String& strName ) const throw()
	{
		return (void*)DYNLIB_GETSYM( m_hInst, strName.c_str() );
	}
	//-------------------------------------------------------------------------------//--
	String DynLib::dynlibError( void ) 
	{
#if WIN32
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			GetLastError(), 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		String ret = (char*)lpMsgBuf;
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return ret;
#else
		return String("");
#endif
	}

}