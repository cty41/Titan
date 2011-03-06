#include "D3D9Texture.h"
#include "PerlinNoise.h"
#include "D3D9Mappings.h"
#include "D3D9Renderer.h"

namespace Titan
{
	D3D9Texture::D3D9Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
		: Texture(mgr, name, id, group), m2DTexture(0)
	{

	}
	//-------------------------------------------------------------//
	D3D9Texture::~D3D9Texture()
	{
		if(mType == TT_2D)
		{
			SAFE_RELEASE(m2DTexture);
		}
	}
	//-------------------------------------------------------------//
	void D3D9Texture::loadImpl()
	{
		if(mType == TT_2D)
			_loadNormalTex();
		else if(mType = TT_CUBEMAP)
			_loadCubeTex();
		
	}
	//-------------------------------------------------------------//
	void D3D9Texture::unloadImpl()
	{
		if(mType == TT_2D)
		{
			SAFE_RELEASE(m2DTexture);
		}
	}
	//-------------------------------------------------------------//
	void D3D9Texture::_loadNormalTex()
	{
		HRESULT hr ;

		DWORD usage = 0;
		if(mUsage == TU_DYNAMIC)
			usage |= D3DUSAGE_DYNAMIC;
		else if(mUsage == TU_RENDERTARGET)
			usage |= D3DUSAGE_RENDERTARGET;

		LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
		if(FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(pD3D9Device, mPreparedData->getPtr(), mPreparedData->size(),
			mWidth, mHeight, mMipmapsLevel,
			usage, D3D9Mappings::convertD3D9Format(mPixelFormat),
			D3D9Mappings::convertD3D9Pool(mPool),
			0, 0, 0, 0, 0, 
			&m2DTexture)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				" create texture failed because of " + errMsg ,
				"D3D9Texture::_loadNormalTex()");
		}

	}
	//-------------------------------------------------------------//
	void D3D9Texture::_loadCubeTex()
	{

	}
	//-------------------------------------------------------------//
	void D3D9Texture::_createCoreObject()
	{
		if(mType == TT_2D)
			_create2DTex();

	}
	//-------------------------------------------------------------//
	void D3D9Texture::_create2DTex()
	{
		DWORD usage = 0;
		if(mUsage == TU_DYNAMIC)
			usage |= D3DUSAGE_DYNAMIC;
		else if(mUsage == TU_RENDERTARGET)
			usage |= D3DUSAGE_RENDERTARGET;

		LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
		HRESULT hr ;
		if(FAILED( hr = D3DXCreateTexture(pD3D9Device, 
			mWidth, mHeight,
			mMipmapsLevel,
			usage, D3D9Mappings::convertD3D9Format(mPixelFormat),
			D3D9Mappings::convertD3D9Pool(mPool),
			&m2DTexture)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3DXCreateTexture failed: " + errMsg, "D3D9Texture::_create2DTex()");
		}
	}
	//-------------------------------------------------------------//
	void D3D9Texture::_perlinNoiseImpl(float scale, int octaves, float falloff)
	{
		PerlinPacket packet;
		packet.scale = scale;
		packet.octaves = octaves;
		packet.falloff = falloff;
		packet.sizeX = (uint16)mWidth;
		packet.sizeY = (uint16)mHeight;

		if (m2DTexture)
		{
			HRESULT hr;
			if(FAILED(hr = D3DXFillTexture(          
				m2DTexture,
				perlinCallback,
				(LPVOID) &packet)))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
					"generate Perlin Noise Texture failed :" + errMsg,
						"D3D9Texture::_perlinNoiseImpl")
			}
		}
	}

	void  WINAPI perlinCallback(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, const D3DXVECTOR2* pTexelSize, void* pData)
	{
		PerlinPacket* pNoise = (PerlinPacket*)pData;
		float x = (pTexCoord->x * pNoise->sizeX);
		float y = (pTexCoord->y * pNoise->sizeY);

		//	float accum = pNoise->perlin.noise(x,y,pNoise->scale);
		//	accum *= 2.0f;

		float accum = 0;
		float frequency = pNoise->scale;//*2.0f;
		float amplitude = 1.0f;

		for (int i=0; i<pNoise->octaves; ++i)
		{
			accum += pNoise->perlin.noise(x,y,frequency) * amplitude;
			amplitude *= pNoise->falloff;
			frequency *= 2.0f;
		}

		accum = Math::Clamp(accum, -1.0f, 1.0f);
		accum *= 0.5f;
		accum += 0.5f;

		pOut->x = accum;
		pOut->y = accum;
		pOut->z = accum;
		pOut->w = accum;
	}
}