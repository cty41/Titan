#ifndef __TITAN_FONT_MGR__HH
#define __TITAN_FONT_MGR__HH

#include "TitanPrerequisites.h"
#include "TitanFont.h"
#include "TitanResourceManager.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport FontMgr : public ResourceMgr, public Singleton<FontMgr>
	{
	public:
		FontMgr();

		~FontMgr();



	public:
		static FontMgr& getSingleton();
		static FontMgr* getSingletonPtr();
	};
}

#endif