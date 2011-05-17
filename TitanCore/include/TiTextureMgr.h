#ifndef _TITAN_TEXTUREMGR_HH
#define _TITAN_TEXTUREMGR_HH

#include "TiPrerequisites.h"
#include "TiTexture.h"
#include "TiResourceMgr.h"
#include "TiSingleton.h"

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


		TexturePtr		load(const String& name, const String& group, TexType texType = TT_2D, int numMipmaps = 1, PixelFormat desiredFormat = PF_UNKNOWN, TexUsage usage = TU_DEFAULT, TexPool pool = TP_DEFAULT);

	protected:
	public:
		static TextureMgr& getSingleton();
		static TextureMgr* getSingletonPtr();

	};
}
#endif