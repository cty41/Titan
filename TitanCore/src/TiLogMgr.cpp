#include "TitanStableHeader.h"
#include "TiLogMgr.h"
#include "TiException.h"
#include <fcntl.h>
#include <io.h>
#include <Windows.h>

namespace Titan
{

	template<> LogMgr* Singleton<LogMgr>::ms_Singleton = 0;

	LogMgr* LogMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}
	LogMgr& LogMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	//-------------------------------------------------------------------------------//
	LogMgr::LogMgr(const String& logFile /* = "Log.ini" */, bool enableCon /* = true */, bool enableVS /* = true */)
		:mLogName(logFile), mConsoleEnable(enableCon), mVSOutput(enableVS), mEnableFileLog(true)
	{
		if(mConsoleEnable)
			enableConsole();

		if(logFile == "")
			mEnableFileLog = false;
		if(mEnableFileLog)
		{
			fopen_s(&mpFile, mLogName.c_str(), "w");
		}
	}
	//-------------------------------------------------------------------------------//
	LogMgr::~LogMgr()
	{
		if(mConsoleEnable)
			disableConsole();

		if(mEnableFileLog)
		{
			fclose(mpFile);
		}

	}
	//-------------------------------------------------------------------------------//
	void LogMgr::enableConsole()
	{
		static const WORD MAX_CONSOLE_LINES = 500;
		int hConHandle;
		long lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE *fp;
		// allocate a console for this app
		AllocConsole();
		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
			coninfo.dwSize);
		// redirect unbuffered STDOUT to the console
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
		setvbuf( stdout, NULL, _IONBF, 0 );
		// redirect unbuffered STDIN to the console
		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "r" );
		*stdin = *fp;
		setvbuf( stdin, NULL, _IONBF, 0 );
		// redirect unbuffered STDERR to the console
		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "w" );
		*stderr = *fp;
		setvbuf( stderr, NULL, _IONBF, 0 );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
	}
	//-------------------------------------------------------------------------------//
	void LogMgr::disableConsole()
	{
		FreeConsole();
	}
	//-------------------------------------------------------------------------------//
	void LogMgr::outputMessage(const String& message, LogMessageLevel cml)
	{
		if(mConsoleEnable)
		{
			std::cout<< message;
		}

		if(mVSOutput)
		{
			OutputDebugString(message.c_str());
		}

		if(mEnableFileLog)
		{
			fprintf_s(mpFile, "%s", message.c_str());
		}
	}
	//-------------------------------------------------------------------------------//
	LogMgr::Stream	LogMgr::stream(LogMessageLevel cml)
	{
		return Stream(ms_Singleton, cml);
	}
	
}
