#ifndef __TITAN_FONT_MGR__HH
#define __TITAN_FONT_MGR__HH

#include "TitanPrerequisites.h"
#include "TitanFont.h"
#include "TiResourceMgr.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport FontMgr : public ResourceMgr, public Singleton<FontMgr>
	{
	public:
		FontMgr();

		~FontMgr();

	protected:
		Resource*	createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams);


	public:
		static FontMgr& getSingleton();
		static FontMgr* getSingletonPtr();
	};
}

#endif