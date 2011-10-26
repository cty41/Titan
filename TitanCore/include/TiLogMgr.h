#ifndef __TITAN_LOGMGR__HH
#define __TITAN_LOGMGR__HH

#include "TiPrerequisites.h"
#include "TiSingleton.h"
#include "TiException.h"

namespace Titan
{
	enum LogMessageLevel
	{
		LML_NORMAL,
		LML_CRITICAL
	};
	//used when we use visual studio as IDE and win32 project
	class _DllExport LogMgr : public Singleton<LogMgr>, public GeneralAlloc
	{
	protected:
		FILE*	mpFile;
		String mLogName;
		bool mConsoleEnable;
		bool mVSOutput;
		bool mEnableFileLog;


	public:
		class Stream;

		LogMgr(const String& logFile = "Log.ini", bool enableConsole = true, bool enableVS = true);

		~LogMgr();
		
		void enableConsole();

		void disableConsole();

		void outputMessage(const String& message, LogMessageLevel cml = LML_NORMAL);

		LogMgr::Stream stream(LogMessageLevel cml = LML_NORMAL);

		static LogMgr* getSingletonPtr();

		static LogMgr& getSingleton();

	public:
		class  Stream 
		{
		protected:
			LogMessageLevel		mLevel;
			StringStream			mCache;
			LogMgr*		mCreator;

		public:

			Stream(LogMgr* creator, LogMessageLevel cml)
				: mCreator(creator), mLevel(cml)
			{
			}
			//-------------------------------------------------------------------------------//
			Stream(const Stream& rhs)
				:mCreator(rhs.mCreator), mLevel(rhs.mLevel)
			{
				mCache.str(rhs.mCache.str());
			}
			//-------------------------------------------------------------------------------//
			~Stream()
			{
				if(mCache.tellp() > 0)
				{
					StringStream cache;
					struct tm *pTime ;
					time_t ctTime; time(&ctTime);
					pTime = localtime(&ctTime );
					cache<<std::setw(2) << std::setfill('0') << pTime->tm_hour
						<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
						<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
						<< ": "<<mapEnumToString(mLevel)<<mCache.rdbuf()<<std::endl;

					mCreator->outputMessage(cache.str(), mLevel);
				}
			}
			//-------------------------------------------------------------------------------//
			template <typename T>
			Stream& operator<< (const T& v)
			{
				mCache << v;
				return *this;
			}

			String mapEnumToString(LogMessageLevel level)
			{
				switch(level)
				{
				case LML_CRITICAL:
					return "Exception: ";
				case LML_NORMAL:
				default:
					return "Log: ";
				}
			}

		};
		

	};

#define DEBUG_LOG Titan::LogMgr::getSingletonPtr()->stream()
#define ERROR_LOG Titan::LogMgr::getSingletonPtr()->stream(LML_CRITICAL)
}

#endif