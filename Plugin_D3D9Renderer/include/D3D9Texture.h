#ifndef _TITAN_D3D9TEXTURE_HH
#define _TITAN_D3D9TEXTURE_HH

#include "D3D9Prerequisites.h"
#include "TiTexture.h"

namespace Titan
{
	class _D3D9DllExport D3D9Texture : public Texture
	{
	public:
		D3D9Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual);

		~D3D9Texture();

		//super class override [begin]
		virtual void lockRect(uint level, const Box* rect, LockOptions options, PixelBox* lockRect);

		virtual void unlockRect(uint level);

		virtual void save(const String& filename);

		virtual void _createCoreObject();
		//super class override [End]

		IDirect3DBaseTexture9* getD3dTexture() const;

	protected:
		//super class override [begin]
		virtual void loadImpl();

		virtual void unloadImpl();

		virtual void _loadImgsImpl(const ConstImagePtrList& images);

		virtual void _perlinNoiseImpl(float scale, int octaves, float falloff);
		//super class override [End]

		void	_loadNormalTex();

		void	_loadCubeTex();

		void	_create2DTex();

		void	_create3DTex();

		void	_createCubeTex();

		void	freeD3d9Tex();


	protected:
		union	TexUnion
		{
			IDirect3DTexture9*	pTex;
			IDirect3DCubeTexture9*	pCubeTex;
			IDirect3DVolumeTexture9*	pVolTex;
		};

		TexUnion	mTexUnion;

	};

	void _D3D9DllExport WINAPI perlinCallback(D3DXVECTOR4* pOut, 
		const D3DXVECTOR2* pTexCoord, 
		const D3DXVECTOR2* pTexelSize, void* pData);

}
#endif