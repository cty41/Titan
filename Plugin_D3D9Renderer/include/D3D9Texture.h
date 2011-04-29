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

		void lockRect(uint level, PixelBox* lockRect, const Box* rect, LockOptions options);

		void unlockRect(uint level);

		IDirect3DBaseTexture9* getD3dTexture() const;

	protected:
		void	loadImpl();

		void	unloadImpl();

		void	prepareImpl();

		void	unprepareImpl();

		void	_loadNormalTex();

		void	_loadCubeTex();

		void	_perlinNoiseImpl(float scale, int octaves, float falloff);

		virtual void	_createCoreObject();

		void	_create2DTex();

		void	_create3DTex();

		void	_createCubeTex();

		virtual void _loadImgsImpl(const ConstImagePtrList& images);

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