#ifndef _TITAN_D3D9TEXTURE_HH
#define _TITAN_D3D9TEXTURE_HH

#include "D3D9Prerequisites.h"
#include "TitanTexture.h"

namespace Titan
{
	class _D3D9DllExport D3D9Texture : public Texture
	{
	public:
		D3D9Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group);

		~D3D9Texture();

	protected:
		void loadImpl();

		void unloadImpl();

		void loadNormalTex();

		void loadCubeTex();

	protected:
		IDirect3DBaseTexture9*		mTexture;
		
	};
}
#endif