#ifndef __D3D11_TEXTURE__HH
#define __D3D11_TEXTURE__HH

#include "D3D11Prerequisites.h"
#include "TiTexture.h"

namespace Titan
{
	class _D3D11DllExport D3D11Texture : public Texture
	{
	public:
		D3D11Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual);

		~D3D11Texture();

		//super class override
		virtual void lockRect(uint level, PixelBox* lockRect, const Box* rect, LockOptions options);

		virtual void unlockRect(uint level);

		virtual void save(const String& filename);

		virtual void _createCoreObject();
		//super class override [End]

	protected:
		//super class override [begin]
		virtual void loadImpl();

		virtual void unloadImpl();

		virtual void _loadImgsImpl(const ConstImagePtrList& images);
		//hack method to generate perlin noise
		void	_perlinNoiseImpl(float scale, int octaves, float falloff);
		//super class override [End]

		void	_loadNormalTex();

		void	_loadCubeTex();

		void	_create2DTex();

		void	_create3DTex();

		void	_createCubeTex();

		void	freeD3d11Tex();

	protected:

	};
}
#endif