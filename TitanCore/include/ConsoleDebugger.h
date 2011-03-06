#ifndef __TITAN_CONSOLEDEBUGGER__HH
#define __TITAN_CONSOLEDEBUGGER__HH

#include "TitanPrerequisites.h"
#include "Singleton.h"
#include "Exception.h"

namespace Titan
{
	enum ConsoleMessageLevel
	{
		CML_NORMAL,
		CML_CRITICAL
	};
	//used when we use visual studio as IDE and win32 project
	class _DllExport ConsoleDebugger : public Singleton<ConsoleDebugger>, public GeneralAlloc
	{
	protected:
		bool mConsoleEnable;

	public:
		class Stream;

		ConsoleDebugger();

		~ConsoleDebugger();
		
		void enableConsole();

		void disableConsole();

		void outputMessage(const String& message, ConsoleMessageLevel cml = CML_NORMAL, bool outputTime = true);

		ConsoleDebugger::Stream stream(ConsoleMessageLevel cml = CML_NORMAL);

		static ConsoleDebugger* getSingletonPtr();

		static ConsoleDebugger& getSingleton();

	public:
		class  Stream 
		{
		protected:
			ConsoleMessageLevel		mLevel;
			StringStream			mCache;

		public:

			Stream(ConsoleMessageLevel cml)
				: mLevel(cml)
			{
			}
			//-------------------------------------------------------------//
			Stream(const Stream& rhs)
				:mLevel(rhs.mLevel)
			{
				mCache.str(rhs.mCache.str());
			}
			//-------------------------------------------------------------//
			~Stream()
			{
				if(mCache.tellp() > 0)
				{
					ConsoleDebugger::getSingletonPtr()->outputMessage(mCache.str(), mLevel);
				}
			}
			//-------------------------------------------------------------//
			template <typename T>
			Stream& operator<< (const T& v)
			{
				mCache << v;
				return *this;
			}

		};
		

	};
}

#endif