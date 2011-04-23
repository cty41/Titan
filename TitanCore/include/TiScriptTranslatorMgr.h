#ifndef _TITAN_SCRIPT_TRANSLATOR_MGR__HH
#define _TITAN_SCRIPT_TRANSLATOR_MGR__HH

#include "TiPrerequisites.h"
#include "TiScriptTranslator.h"
#include "TiScriptCompilerMgr.h"


namespace Titan
{
	typedef ScriptCompilerMgr::ScriptNodePtr ScriptNodePtr;
	class _DllExport ScriptTranslatorMgr : public GeneralAlloc
	{
	public:
		typedef map<String, ScriptTranslator*>::type ScriptTranslatorMap;

	public:
		ScriptTranslatorMgr();

		~ScriptTranslatorMgr();

		void addTranslator(ScriptTranslator* translator);

		void removeAllTranslator();


		ScriptTranslator*	getTranslator(const String& name);

		void translate(ScriptNodePtr& nodePtr, const String& group);

	protected:
		ScriptTranslatorMap		mScriptTranslatorMap;
	};
}
#endif