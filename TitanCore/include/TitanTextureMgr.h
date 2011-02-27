#ifndef _TITAN_TEXTUREMGR_HH
#define _TITAN_TEXTUREMGR_HH

#include "TitanPrerequisites.h"
#include "TitanTexture.h"
#include "TitanResourceManager.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport TextureMgr : public ResourceMgr, public Singleton<TextureMgr>
	{
	public:
		TextureMgr();

		~TextureMgr();
	protected:
	public:
		static TextureMgr& getSingleton();
		static TextureMgr* getSingletonPtr();

	};
}
#endif