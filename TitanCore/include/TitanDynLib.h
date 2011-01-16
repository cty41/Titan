#ifndef _TITAN_DYNLIB_HH
#define _TITAN_DYNLIB_HH

#include "TitanPrerequisites.h"

#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) !FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

namespace Titan
{
	class _DllExport DynLib : public GeneralAlloc
	{
	public:
		DynLib(const String& name);

		~DynLib();

		void load();

		void unload();

		const String& getName(void) const { return mName; }

		void* getSymbol( const String& strName ) const throw();

	protected:
		String mName;
		/// Gets the last loading error
		String dynlibError(void);
		/// Handle to the loaded library.
		DYNLIB_HANDLE m_hInst;
	};
}

#endif