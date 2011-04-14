#ifndef _TITAN_TEXTUREMGR_HH
#define _TITAN_TEXTUREMGR_HH

#include "TitanPrerequisites.h"
#include "TitanTexture.h"
#include "TiResourceMgr.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport TextureMgr : public ResourceMgr, public Singleton<TextureMgr>
	{
	public:
		TextureMgr();

		~TextureMgr();

		TexturePtr	createManually(const String& name, const String& group, 
			TexType type, uint width, uint height, uint mipmapLevel,TexUsage usage, PixelFormat format,
			TexPool pool = TP_DEFAULT);

	protected:
	public:
		static TextureMgr& getSingleton();
		static TextureMgr* getSingletonPtr();

	};
}
#endif