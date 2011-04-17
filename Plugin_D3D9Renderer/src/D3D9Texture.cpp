#include "D3D9Texture.h"
#include "TiPerlinNoise.h"
#include "D3D9Mappings.h"
#include "D3D9Renderer.h"
#include "TitanNumericTools.h"
#include "TiException.h"
#include "TiResourceGroupMgr.h"

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
	void fromD3DLock(PixelBox &rval, const D3DLOCKED_RECT &lrect)
	{
		size_t bpp = PixelFuncs::getNumElemBytes(rval.format);
		if (bpp != 0)
		{
			rval.rowPitch = lrect.Pitch / bpp;
			rval.slicePitch = rval.rowPitch * rval.getHeight();
			assert((lrect.Pitch % bpp)==0);
		}
		//add compressed later
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"Invalid pixel format", "fromD3DLock");
		}

		rval.data = lrect.pBits;
	}
	void fromD3DLock(PixelBox &rval, const D3DLOCKED_BOX &lbox)
	{
		size_t bpp = PixelFuncs::getNumElemBytes(rval.format);
		if (bpp != 0)
		{
			rval.rowPitch = lbox.RowPitch / bpp;
			rval.slicePitch = lbox.SlicePitch / bpp;
			assert((lbox.RowPitch % bpp)==0);
			assert((lbox.SlicePitch % bpp)==0);
		}
		//add compressed later
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"Invalid pixel format", "fromD3DLock");
		}
		rval.data = lbox.pBits;
	}
	// Convert Ogre integer Box to D3D rectangle
	RECT toD3DRECT(const Box &lockBox)
	{
		RECT prect;
		assert(lockBox.getDepth() == 1);
		prect.left = static_cast<LONG>(lockBox.left);
		prect.right = static_cast<LONG>(lockBox.right);
		prect.top = static_cast<LONG>(lockBox.top);
		prect.bottom = static_cast<LONG>(lockBox.bottom);
		return prect;
	}
	// Convert Ogre integer Box to D3D box
	D3DBOX toD3DBOX(const Box &lockBox)
	{
		D3DBOX pbox;

		pbox.Left = static_cast<UINT>(lockBox.left);
		pbox.Right = static_cast<UINT>(lockBox.right);
		pbox.Top = static_cast<UINT>(lockBox.top);
		pbox.Bottom = static_cast<UINT>(lockBox.bottom);
		pbox.Front = static_cast<UINT>(lockBox.front);
		pbox.Back = static_cast<UINT>(lockBox.back);
		return pbox;
	}
	// Convert Ogre pixelbox extent to D3D rectangle
	RECT toD3DRECTExtent(const PixelBox &lockBox)
	{
		RECT prect;
		assert(lockBox.getDepth() == 1);
		prect.left = 0;
		prect.right = static_cast<LONG>(lockBox.getWidth());
		prect.top = 0;
		prect.bottom = static_cast<LONG>(lockBox.getHeight());
		return prect;
	}
	// Convert Ogre pixelbox extent to D3D box
	D3DBOX toD3DBOXExtent(const PixelBox &lockBox)
	{
		D3DBOX pbox;
		pbox.Left = 0;
		pbox.Right = static_cast<UINT>(lockBox.getWidth());
		pbox.Top = 0;
		pbox.Bottom = static_cast<UINT>(lockBox.getHeight());
		pbox.Front = 0;
		pbox.Back = static_cast<UINT>(lockBox.getDepth());
		return pbox;
	}
	//-------------------------------------------------------------//
	void D3D9Texture::lockRect(uint level, PixelBox* lockRect, const Box* rect, LockOptions options)
	{
		assert(lockRect != NULL&& "lockRect should not be zero pointer");

		HRESULT hr;
		DWORD flags = 0;
		switch(options)
		{
		case HardwareBuffer::HBL_DISCARD:
			// D3D only likes D3DLOCK_DISCARD if you created the texture with D3DUSAGE_DYNAMIC
			// debug runtime flags this up, could cause problems on some drivers
			if (mUsage & TU_DYNAMIC)
				flags |= D3DLOCK_DISCARD;
			break;
		case HardwareBuffer::HBL_READ_ONLY:
			flags |= D3DLOCK_READONLY;
			break;
		default: 
			break;
		};
		if(mType == TT_2D)
		{
			D3DLOCKED_RECT lockedRect;
			
			if(rect != NULL)
			{
				RECT _rect = toD3DRECT(*rect);
				hr = m2DTexture->LockRect(level, &lockedRect, &_rect, flags);
			}
			else
				hr = m2DTexture->LockRect(level, &lockedRect, NULL, flags);
			if(FAILED(hr))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Texture lock rect failed because of: " + errMsg,
					"D3D9Texture::lockRect");
			}
			lockRect->format = mPixelFormat;
			fromD3DLock(*lockRect, lockedRect);
		}

	}
	//-------------------------------------------------------------//
	void D3D9Texture::unlockRect(uint level)
	{
		HRESULT hr;
		if(mType == TT_2D)
		{
			if(FAILED(hr = m2DTexture->UnlockRect(level)))
			{
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Texture unlock rect failed",
					"D3D9Texture::unlockRect");
			}
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
	//------------------------------------------------------------------------------//
	void D3D9Texture::prepareImpl()
	{
		DataStreamPtr dataStream = ResourceGroupMgr::getSingltonPtr()->openResource(mName, mGroup);
		mPreparedData = MemoryDataStreamPtr(TITAN_NEW MemoryDataStream(dataStream));
	}
	//------------------------------------------------------------------------------//
	void D3D9Texture::unprepareImpl()
	{
		mPreparedData.setNull();
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

		D3DFORMAT d3dPF = D3D9Mappings::convertD3D9Format(mPixelFormat);
		D3DPOOL d3dPool = D3D9Mappings::convertD3D9Pool(mPool);
		if(FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(pD3D9Device, (void*)mPreparedData->getPtr(), mPreparedData->size(),
			mWidth, mHeight, mMipmapsLevel,
			usage, 
			d3dPF,
			d3dPool,
			D3DX_DEFAULT, D3DX_DEFAULT, 
			0, NULL, NULL, 
			&m2DTexture)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				" create texture failed because of " + errMsg ,
				"D3D9Texture::_loadNormalTex()");
		}

		D3DSURFACE_DESC texDesc;
		m2DTexture->GetLevelDesc(0, & texDesc);
		mWidth = texDesc.Width;
		mHeight = texDesc.Height;
		mPixelFormat = D3D9Mappings::convertToTitan(texDesc.Format);
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

		accum = clamp(accum, -1.0f, 1.0f);
		accum *= 0.5f;
		accum += 0.5f;

		pOut->x = accum;
		pOut->y = accum;
		pOut->z = accum;
		pOut->w = accum;
	}
}