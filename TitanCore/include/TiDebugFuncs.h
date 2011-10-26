#ifndef __TITAN_DEBUGFUNCS__HH
#define __TITAN_DEBUGFUNCS__HH

#include <Windows.h>

//#define appIsDebuggerPresent	IsDebuggerPresent
//#define appDebugBreak()			( appIsDebuggerPresent() ? (DebugBreak(),1) : 1 )
//
//void cdecl appFailAssertFuncDebug( const char* Expr, const char* File, int Line, const char* Format = "", ... );
//
//#define appFailAssert(expr,file,line,...)				{ if (appIsDebuggerPresent()) appFailAssertFuncDebug(expr, file, line, ##__VA_ARGS__); appDebugBreak(); appFailAssertFunc(expr, file, line, ##__VA_ARGS__); }

#endif