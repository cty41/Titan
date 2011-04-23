#ifndef _TITAN_SCRIPT_LOADER__H
#define _TITAN_SCRIPT_LOADER__H

#include "TiPrerequisites.h"
#include "TiDataStream.h"

namespace Titan
{
	class _DllExport ScriptLoader
	{
	public:
		ScriptLoader(){}

		virtual ~ScriptLoader(){}

		//get filename extensions
		virtual const StringVector& getScriptExts() const = 0;

		virtual int getScriptLoaderPriority() const = 0;

		virtual void parseScript(DataStreamPtr& stream, const String& group) = 0;
	};
}

#endif