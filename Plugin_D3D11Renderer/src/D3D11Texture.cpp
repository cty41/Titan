#include "D3D11Texture.h"
#include "D3D11Renderer.h"
#include "D3D11Mappings.h"

namespace Titan
{
	D3D11Texture::D3D11Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual)
		: Texture(mgr, name, id, group, isManual), mTex1D(nullptr), mTex2D(nullptr), mTex3D(nullptr), mShaderResView(nullptr)
	{
	}
	//-------------------------------------------------------------//
	D3D11Texture::~D3D11Texture()
	{
		freeD3d11Tex();
	}

	void D3D11Texture::lockRect(uint level, const Box* rect, LockOptions options, PixelBox* lockRect)
	{
		if (lockRect == nullptr)
		{
			TITAN_EXCEPT_API("D3D11: lockRect can not be null!");
			return ;
		}

		HRESULT hr;
		D3D11_MAP flags;
		switch(options)
		{
		case HardwareBuffer::HBL_DISCARD:
			if (mUsage & TU_DYNAMIC)
				flags = D3D11_MAP_WRITE_DISCARD;

			break;
		case HardwareBuffer::HBL_READ_ONLY:
			flags = D3D11_MAP_READ;
			break;
		default: 
			break;
		};

		ID3D11DeviceContext* pd3dContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
		D3D11_MAPPED_SUBRESOURCE d11SubRes;
		ID3D11Resource* pTexRes = nullptr;
		if(mType == TT_2D)
		{
			pTexRes = mTex2D;
		}
		//todo, add support for other type textures

		hr = pd3dContext->Map(pTexRes, (UINT)level, flags, NULL, &d11SubRes);

		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "D3D11 lock texture failed:");
			return ;
		}

		lockRect->format = mPixelFormat;
		lockRect->data = d11SubRes.pData;
		size_t bpp = PixelFuncs::getNumElemBytes(mPixelFormat);
		lockRect->rowPitch = d11SubRes.RowPitch / bpp;
		
	}

	void D3D11Texture::unlockRect(uint level)
	{
		ID3D11DeviceContext* pd3dContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
		ID3D11Resource* pTexRes = nullptr;
		if(mType == TT_2D)
		{
			pTexRes = mTex2D;
		}
		//todo, add support for other type textures
		pd3dContext->Unmap(pTexRes, (UINT)level);
	}

	void D3D11Texture::save(const String& filename)
	{

	}


	//super class override [begin]
	void D3D11Texture::loadImpl()
	{
		if(mType == TT_2D)
			_loadNormalTex();
		else if(mType = TT_CUBEMAP)
			_loadCubeTex();
	}

	void D3D11Texture::unloadImpl()
	{
		freeD3d11Tex();
	}

	void D3D11Texture::_createCoreObject()
	{
		if(mType == TT_2D || mType == TT_1D)
			_create2DTex();
		else if(mType == TT_3D)
			_create3DTex();
		else
			_createCubeTex();
	}

	void D3D11Texture::_loadImgsImpl(const ConstImagePtrList& images)
	{

	}

	void D3D11Texture::_perlinNoiseImpl(float scale, int octaves, float falloff)
	{

	}

	void D3D11Texture::_loadNormalTex()
	{
		HRESULT hr;
		ID3D11Device*	pd3dDevice = D3D11Renderer::getSingletonPtr()->__getD3D11Device();

		
		//load text info first
		D3DX11_IMAGE_INFO info;
		D3DX11GetImageInfoFromMemory(mPreparedData->getPtr(), mPreparedData->size(),
			nullptr, &info, &hr);

		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "d3d11 load 2d texture info failed : ");
			return ;
		}

		
		D3DX11_IMAGE_LOAD_INFO texInfo;
		texInfo.Width = info.Width; 
		texInfo.Height = info.Height;
		texInfo.Depth = mDepth;
		texInfo.Format = info.Format;

		texInfo.FirstMipLevel = 0; 
		texInfo.MipLevels = info.MipLevels;
		if(mUsage == TU_DEFAULT)
			texInfo.Usage = D3D11_USAGE_DEFAULT;
		else
			texInfo.Usage = D3D11_USAGE_DYNAMIC;
		texInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texInfo.CpuAccessFlags = 0;
		texInfo.MiscFlags = info.MiscFlags;
		texInfo.Filter = D3DX11_DEFAULT;
		texInfo.MipFilter = D3DX11_DEFAULT;
		texInfo.pSrcInfo = nullptr;

		
		D3DX11CreateTextureFromMemory(
			pd3dDevice, mPreparedData->getPtr(), mPreparedData->size(),
			&texInfo, nullptr, (ID3D11Resource**)&mTex2D, &hr);

		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "d3d11 load 2d texture failed : ");
		}

		D3D11_TEXTURE2D_DESC desc;
		mTex2D->GetDesc(&desc);

		memset(&mSRVDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		mSRVDesc.Format = desc.Format;
		mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		mSRVDesc.Texture2D.MipLevels = desc.MipLevels;

		hr = pd3dDevice->CreateShaderResourceView(mTex2D, &mSRVDesc, &mShaderResView);
		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "D3DX create shader resource view failed: ");
		}

		mWidth = desc.Width; mHeight = desc.Height; mDepth = 1;
		mPixelFormat = D3D11Mappings::convertToTitan(desc.Format);
		mMipmapsLevel = desc.MipLevels;
	}

	void D3D11Texture::_loadCubeTex()
	{

	}

	void D3D11Texture::_create2DTex()
	{
		HRESULT hr;
		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0 , sizeof(desc));
		desc.ArraySize = 1;
		desc.Width = mWidth; desc.Height = mHeight; 
		desc.Format = D3D11Mappings::convertToD3D11(mPixelFormat);
		desc.MipLevels = mMipmapsLevel;
		desc.SampleDesc.Count = 1;
		if(mUsage == TU_DEFAULT)
			desc.Usage = D3D11_USAGE_DEFAULT;
		else
			desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		
		ID3D11Device*	pd3dDevice = D3D11Renderer::getSingletonPtr()->__getD3D11Device();
		hr = pd3dDevice->CreateTexture2D(&desc, nullptr, &mTex2D);
		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "D3DX11 Create 2D texture failed: ");
		}
	}

	void D3D11Texture::_create3DTex()
	{

	}

	void D3D11Texture::_createCubeTex()
	{

	}

	//super class override [End]

	ID3D11Resource*	D3D11Texture::getD11Texture() const 
	{
		if(mType == TT_2D)
			return mTex2D;
		else if(mType == TT_1D)
			return mTex1D;
		else if (mType ==TT_3D)
			return mTex3D;

		//ease complier warning
		return mTex2D;
	}

	void D3D11Texture::freeD3d11Tex()
	{
		SAFE_RELEASE(mShaderResView);
		SAFE_RELEASE(mTex1D);
		SAFE_RELEASE(mTex2D);	
		SAFE_RELEASE(mTex3D);	
	}
}
