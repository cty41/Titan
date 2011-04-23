#ifndef __TITAN_PLUGIN_HH
#define __TITAN_PLUGIN_HH

#include "TiPrerequisites.h"

namespace Titan
{
	class _DllExport Plugin : public GeneralAlloc
	{
	public:
		Plugin(){};

		virtual ~Plugin(){};

		virtual const String& getName() const = 0;

		virtual void install() = 0;

		virtual void initialise() = 0;

		virtual void shutdown() = 0;

		virtual void uninstall() = 0;
	};
}

#endif