#include "TitanStableHeader.h"
#include "TiViewport.h"
#include "TiTexture.h"

#include "D3D11Renderer.h"
#include "D3D11RenderWindow.h"
#include "D3D11Mappings.h"
#include "D3D11VertexDecl.h"
#include "D3D11HLSLShader.h"
#include "D3D11HardwareBufferMgr.h"
#include "D3D11ShaderMgr.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "D3D11TextureMgr.h"
#include "D3D11Texture.h"

namespace Titan
{

	template<> D3D11Renderer* Singleton<D3D11Renderer>::ms_Singleton = 0;

	D3D11Renderer* D3D11Renderer::getSingletonPtr()
	{
		return ms_Singleton;
	}
	D3D11Renderer& D3D11Renderer::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	D3D11Renderer::D3D11Renderer(HINSTANCE hInstance)
		: Renderer(), mhInstance(hInstance),
		mpD3dDevice(nullptr), mpDeviceContext(nullptr), mSwapChain(nullptr), 
		mRenderTargetView(nullptr), mDepthStencilTex(nullptr), mDepthStencilView(nullptr),
		mStencilRef(0), mFeatureLevel(D3D_FEATURE_LEVEL_9_1), mBoundVertexShader(nullptr), mBoundPixelShader(nullptr),
		mCachePrimitiveType(OT_TRIANGLE_LIST), mMaxUsingTextureStage(0), 
		mUseBlend(false), mUseRasterizer(false), mUseSampler(false), mUseDepthStencil(false)
	{
		mName = "D3D11Renderer";

		ZeroMemory(&mBlendDesc, sizeof(D3D11_BLEND_DESC));
		mBlendDesc.AlphaToCoverageEnable = false;
		mBlendDesc.IndependentBlendEnable = false;
		ZeroMemory(&mSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		ZeroMemory(&mRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		ZeroMemory(&mDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		ZeroMemory(&mTextureStageInfos, sizeof(mTextureStageInfos));
	}

	D3D11Renderer::~D3D11Renderer()
	{
		//why should I wrap a destroy function instead of just write code in destruction
		destroy();	
	}

	RenderWindow*	D3D11Renderer::initialize()
	{
		mHardwareBufferMgr = TITAN_NEW D3D11HardwareBufferMgr();

		mShaderMgr = TITAN_NEW D3D11ShaderMgr();
		
		mTextureMgr = TITAN_NEW D3D11TextureMgr();

		return Renderer::initialize();
	}

	void D3D11Renderer::destroy()
	{
		//delete state objects
		
		for(auto it = mRasterizerStateMap.begin(); it != mRasterizerStateMap.end(); ++it)
			SAFE_RELEASE(it->second);
		mRasterizerStateMap.clear();

		for(auto it = mSamplerStateMap.begin(); it != mSamplerStateMap.end(); ++it)
			SAFE_RELEASE(it->second);
		mSamplerStateMap.clear();

		for(auto it = mDepthStencilStateMap.begin(); it != mDepthStencilStateMap.end(); ++it)
			SAFE_RELEASE(it->second);
		mDepthStencilStateMap.clear();

		for(auto it = mBlendStateMap.begin(); it != mBlendStateMap.end(); ++it)
			SAFE_RELEASE(it->second);
		mBlendStateMap.clear();

		TITAN_SAFE_DELETE(mTextureMgr);
		TITAN_SAFE_DELETE(mShaderMgr);
		TITAN_SAFE_DELETE(mHardwareBufferMgr);

		SAFE_RELEASE(mDepthStencilTex);
		SAFE_RELEASE(mDepthStencilView);
		SAFE_RELEASE(mRenderTargetView);
		SAFE_RELEASE(mpDeviceContext);
		SAFE_RELEASE(mpD3dDevice);
		SAFE_RELEASE(mSwapChain);
	}

	void D3D11Renderer::_clearFrameBuffer(unsigned int buffers, 
		const Color& color /* = Color::Black */, float depth /* = 1.0f */, unsigned short stencil /* = 0 */)
	{
		if(buffers & FBT_COLOR)
			mpDeviceContext->ClearRenderTargetView(mRenderTargetView, color.ptr());

		UINT dsFlag = 0;
		if(buffers & FBT_DEPTH)
			dsFlag |= D3D11_CLEAR_DEPTH;

		if(buffers & FBT_STENCIL)
			dsFlag |= D3D11_CLEAR_STENCIL;

		if(dsFlag != 0)
			mpDeviceContext->ClearDepthStencilView(mDepthStencilView, dsFlag, depth, (UINT8)stencil);
	}

	void D3D11Renderer::initRenderDevice()
	{
		HRESULT hr;
		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_REFERENCE,
		};

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		UINT numFeatureLevels = sizeof( featureLevels )/ sizeof(D3D_FEATURE_LEVEL);

		for( uint driverTypeIndex = 0; driverTypeIndex < 2; driverTypeIndex++ )
		{
			mDriverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain( NULL, mDriverType, NULL, createDeviceFlags,
				featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &mDesc, &mSwapChain, &mpD3dDevice, &mFeatureLevel, &mpDeviceContext);
			if( FAILED(hr))
			{
				TITAN_EXCEPT_API_D11(hr, "D3DX11 Create Device Error:");
				continue;
			}
			else
				break;
		}

		// Create a render target view
		ID3D11Texture2D* pBuffer;
		hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBuffer );
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API_D11(hr, "Swap Chain getBuffer failed");
		}

		hr = mpD3dDevice->CreateRenderTargetView( pBuffer, NULL, &mRenderTargetView );
		pBuffer->Release();
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API( "Create Render Target View failed");
		}

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));
		descDepth.Width = mWidth;
		descDepth.Height = mHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = mpD3dDevice->CreateTexture2D( &descDepth, NULL, &mDepthStencilTex );
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API_D11(hr, "Create Depth texture failed");
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = mpD3dDevice->CreateDepthStencilView( mDepthStencilTex, &descDSV, &mDepthStencilView );
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API_D11(hr,"CreateDepthStencilView failed");
		}

		mpDeviceContext->OMSetRenderTargets( 1, &mRenderTargetView, mDepthStencilView );
	}

	RenderWindow* D3D11Renderer::createRenderWindow(const String& caption, uint width, uint height, bool fullscreen)
	{
		mWidth = width; mHeight = height;
		mWindow = TITAN_NEW D3D11RenderWindow(mhInstance);
		mWindow->create(caption, width, height, fullscreen);
		mTargetMap.insert(RenderTargetMap::value_type(caption, mWindow));

		D3D11RenderWindow* d11Window= nullptr;
		d11Window = static_cast<D3D11RenderWindow*>(mWindow);
		d11Window->setD3D11Descs(&mDesc);

		//called here because create render device need some infos which is created after window comes
		initRenderDevice();

		d11Window->setSwapChain(mSwapChain);
		addTargetToRender(mWindow);

		return mWindow;
	}

	//-------------------------------------------------------------------------------//
	String D3D11Renderer::getErrorDiscription(long result) const 
	{
		const String errMsg = DXGetErrorDescription(result);
		return errMsg;
	}
	//-------------------------------------------------------------------------------//
	void D3D11Renderer::_setViewport(Viewport* vp)
	{
		D3D11_VIEWPORT d3dvp;
		// set viewport dimensions
		d3dvp.TopLeftX = (FLOAT)vp->getActualLeft();
		d3dvp.TopLeftY = (FLOAT)vp->getActualTop();
		d3dvp.Width = (FLOAT)vp->getActualWidth();
		d3dvp.Height = (FLOAT)vp->getActualHeight();
		d3dvp.MinDepth = 0.0f;
		d3dvp.MaxDepth = 1.0f;

		mpDeviceContext->RSSetViewports( 1, &d3dvp);
	}

	void D3D11Renderer::_setPolygonMode(PolygonMode pm)
	{
		mRasterizerDesc.FillMode = D3D11Mappings::convertToD3D11(pm);
		mUseRasterizer = true;
	}
	//-------------------------------------------------------------------------------//
	void D3D11Renderer::_setCullingMode(CullingMode cm)
	{
		mRasterizerDesc.CullMode = D3D11Mappings::convertToD3D11(cm);
		mUseRasterizer = true;
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setDepthCheck(bool state)
	{
		mDepthStencilDesc.DepthEnable = state;
		mUseDepthStencil = true;
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setDepthWrite(bool state)
	{
		mUseDepthStencil = true;
		if(state)
			mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		else
			mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setDepthFuntion(CompareFunction cf)
	{
		mUseDepthStencil = true;
		mDepthStencilDesc.DepthFunc = D3D11Mappings::convertToD3D11(cf);
	}
	//change later
	void D3D11Renderer::_setSamplerFilter(uint stage, FilterType type, FilterOptions fo)
	{
		assert(stage < TITAN_MAX_TEXTURE_LAYERS);
		mUseSampler = true;
		mTextureStageInfos[stage].samplerDesc.Filter = D3D11Mappings::convertToD3D11(fo);
	}	
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setTexAddressMode(uint stage, const TexAddressModeSets& tam)
	{
		assert(stage < TITAN_MAX_TEXTURE_LAYERS);
		mUseSampler = true;
		mTextureStageInfos[stage].samplerDesc.AddressU = D3D11Mappings::convertToD3D11(tam.UTexAddrMode);
		mTextureStageInfos[stage].samplerDesc.AddressV = D3D11Mappings::convertToD3D11(tam.VTexAddrMode);
		mTextureStageInfos[stage].samplerDesc.AddressW = D3D11Mappings::convertToD3D11(tam.WTexAddrMode);
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setTexCoordSet(uint stage, size_t texcoordSet)
	{
		assert(stage < TITAN_MAX_TEXTURE_LAYERS);
		mUseSampler = true;
		mTextureStageInfos[stage].coordIdx = texcoordSet;
	}

	void D3D11Renderer::_setTexture(size_t stage, const TexturePtr& pTex)
	{
		assert(stage < TITAN_MAX_TEXTURE_LAYERS);
		mUseSampler = true;
		if(mMaxUsingTextureStage < stage) mMaxUsingTextureStage = stage;

		D3D11Texture* pD11Tex = static_cast<D3D11Texture*>(pTex.get());
		mTextureStageInfos[stage].pSRV = pD11Tex->getSRV();
	}

	//-------------------------------------------------------------------------------//
	void D3D11Renderer::_setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			mBlendDesc.RenderTarget[0].BlendEnable = FALSE;
		}
		else
		{
			mBlendDesc.RenderTarget[0].BlendEnable = TRUE;
			mBlendDesc.RenderTarget[0].SrcBlend = D3D11Mappings::convertToD3D11(sourceFactor);
			mBlendDesc.RenderTarget[0].DestBlend = D3D11Mappings::convertToD3D11(destFactor);
			mBlendDesc.RenderTarget[0].BlendOp = D3D11Mappings::convertToD3D11(op);
			mBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11Mappings::convertToD3D11(op);
			mUseBlend = true;
		}

	}

	const String& D3D11Renderer::getName() const
	{
		return mName;
	}

	ID3D11RasterizerState* D3D11Renderer::_getCacheRasterizerState()
	{
		ID3D11RasterizerState* pRasterizerState = nullptr;
		RasterizerState rasterState(mRasterizerDesc);
#if DO_CACHE_D11STATES
		auto it = mRasterizerStateMap.find(rasterState);
		if(it != mRasterizerStateMap.end())
		{
			pRasterizerState = it->second;
		}
		else
#endif
		{
			mpD3dDevice->CreateRasterizerState(&mRasterizerDesc, &pRasterizerState);
#if DO_CACHE_D11STATES
			mRasterizerStateMap[rasterState] = pRasterizerState;
#endif
		}

		return pRasterizerState;
	}

	ID3D11SamplerState* D3D11Renderer::_getCacheSamplerState(uint stage)
	{
		ID3D11SamplerState* pSamplerState = nullptr;
		SamplerState samplerState(mTextureStageInfos[stage].samplerDesc);
#if DO_CACHE_D11STATES
		auto it = mSamplerStateMap.find(samplerState);
		if(it != mSamplerStateMap.end())
		{
			pSamplerState = it->second;
		}
		else
#endif
		{
			mpD3dDevice->CreateSamplerState(&mTextureStageInfos[stage].samplerDesc, &pSamplerState);
#if DO_CACHE_D11STATES
			mSamplerStateMap[samplerState] = pSamplerState;
#endif
		}
		return pSamplerState;
	}

	ID3D11BlendState* D3D11Renderer::_getCacheBlendState()
	{
		ID3D11BlendState* pBlendState = nullptr;
		BlendState blendState(mBlendDesc);
#if DO_CACHE_D11STATES
		auto it = mBlendStateMap.find(blendState);
		if(it != mBlendStateMap.end())
		{
			pBlendState = it->second;
		}
		else
#endif
		{
			mpD3dDevice->CreateBlendState(&mBlendDesc, &pBlendState);
#if DO_CACHE_D11STATES
			mBlendStateMap[blendState] = pBlendState;
#endif
		}
		return pBlendState;
	}

	ID3D11DepthStencilState* D3D11Renderer::_getCacheDepthStencilState()
	{
		ID3D11DepthStencilState* pDSState = nullptr;
		DepthStencilState dsState(mDepthStencilDesc);
#if DO_CACHE_D11STATES
		auto it = mDepthStencilStateMap.find(dsState);
		if(it != mDepthStencilStateMap.end())
		{
			pDSState = it->second;
		}
		else
#endif
		{
			mpD3dDevice->CreateDepthStencilState(&mDepthStencilDesc, &pDSState);
#if DO_CACHE_D11STATES
			mDepthStencilStateMap[dsState] = pDSState;
#endif
		}
		return pDSState;
	}

	/// d3d11 render op, and call all real set state object here, is that right? -cty

	void D3D11Renderer::_render(const RenderData& rd)
	{
		if (rd.vertexData->vertexCount == 0)
			return;

		Renderer::_render(rd);

		if(mUseRasterizer)
			mpDeviceContext->RSSetState(_getCacheRasterizerState());
		if(mUseDepthStencil)
			mpDeviceContext->OMSetDepthStencilState(_getCacheDepthStencilState(), mStencilRef);
		if(mUseBlend)
			mpDeviceContext->OMSetBlendState(_getCacheBlendState(), 0, 0xffffffff);

		if(mUseSampler)
		{
			ID3D11SamplerState* pSamplerState;
			for(uint stage = 0; stage < mMaxUsingTextureStage; ++stage)
			{
				pSamplerState = _getCacheSamplerState(stage);
				mpDeviceContext->PSSetShaderResources(stage, 1, &mTextureStageInfos[stage].pSRV);
				mpDeviceContext->PSSetSamplers(stage, 1,&pSamplerState);
			}
		}


		DWORD primCount = 0;
		D3D10_PRIMITIVE_TOPOLOGY primType;

		switch( rd.operationType )
		{
		case OT_TRIANGLE_LIST:
			primType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) / 3;
			break;
		case OT_TRIANGLE_STRIP:
			primType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 2;
			break;
		}

		if (!primCount)
			return;

		//if(mCachePrimitiveType != rd.operationType)
		{
			mpDeviceContext->IASetPrimitiveTopology(primType);
			//mCachePrimitiveType = rd.operationType;
		}

		//set vertex decl and buffer binding
		setVertexDeclaration(rd.vertexData->vertexDecl);
		setVertexBufferBinding(rd.vertexData->vertexBufferBinding);

		if(rd.useIndex)
		{
			D3D11IndexBuffer* pIndexBuffer = static_cast<D3D11IndexBuffer*>(rd.indexData->indexBuffer.get());
			mpDeviceContext->IASetIndexBuffer(
				pIndexBuffer->getD3D11IndexBuffer(), 
				D3D11Mappings::convertToD3D11(pIndexBuffer->getIndexType()), 
				0 );

			mpDeviceContext->DrawIndexed( rd.indexData->indexCount, rd.indexData->indexStart, rd.vertexData->vertexStart);
		}
		else
		{
			mpDeviceContext->Draw(rd.vertexData->vertexCount, rd.vertexData->vertexStart);
		}

		//after a call of render , we clean the mMaxUsingTextureStage
		mMaxUsingTextureStage = 0;
		mUseRasterizer = false;
		mUseDepthStencil = false;
		mUseSampler = false;
		mUseBlend = false;
	}

	void D3D11Renderer::_beginFrame()
	{
		//todo for d11, use for multi thread?
		//mpDeviceContext->Begin()
	}

	void D3D11Renderer::_endFrame()
	{
		//todo for d11
	}

	void D3D11Renderer::_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram)
	{
		//d11 is right hand system?
		projDst = projSrc;

		//projDst[2][0] = (projDst[2][0] + projDst[3][0]) / 2;
		//projDst[2][1] = (projDst[2][1] + projDst[3][1]) / 2;
		//projDst[2][2] = (projDst[2][2] + projDst[3][2]) / 2;
		//projDst[2][3] = (projDst[2][3] + projDst[3][3]) / 2;
	}

	void D3D11Renderer::_setTextureMatrix(size_t unit, const Matrix4& xform)
	{
		//todo, tex matrix
	}

	void D3D11Renderer::setVertexDeclaration(VertexDeclaration* decl)
	{
		D3D11VertexDecl* d11Decl = static_cast<D3D11VertexDecl*>(decl);
		//dx11 is different from d9 which need to bind vertex decl to shader manually to get better performance
		if(mBoundVertexShader == NULL)
		{
			//todo, use simul
		}
		ID3D11InputLayout* inputLayout =  d11Decl->getD11InputLayoutByShader(mBoundVertexShader);
		//do we need cache here?
		mpDeviceContext->IASetInputLayout(inputLayout);
	}

	void D3D11Renderer::setVertexBufferBinding(VertexBufferBinding* binding)
	{
		const VertexBufferBinding::VertexBufferBindingMap& binds = binding->getBindings();
		UINT idx = 0;
		for(auto it = binds.cbegin(); it != binds.cend(); ++it, ++idx)
		{
			D3D11VertexBuffer* pVB = static_cast<D3D11VertexBuffer*>(it->second.get());
			UINT stride = static_cast<UINT>(pVB->getVertexSize());
			UINT offset = 0;
			ID3D11Buffer* pBuffer = pVB->getD3D11VertexBuffer();
			mpDeviceContext->IASetVertexBuffers(idx, 1, &pBuffer, &stride, &offset);
		}

	}

	void D3D11Renderer::_setShader(Shader* shader)
	{
		
		if(shader->getShaderType() == ST_VERTEX_SHADER)
		{
			D3D11VertexShader* d11Shader = static_cast<D3D11VertexShader*>(shader);
			mBoundVertexShader = d11Shader;
			mpDeviceContext->VSSetShader(d11Shader->getVertexShader(), nullptr, 0);
		}
		else if (shader->getShaderType() == ST_PIXEL_SHADER)
		{
			D3D11PixelShader* d11Shader = static_cast<D3D11PixelShader*>(shader);
			mBoundPixelShader = d11Shader;
			mpDeviceContext->PSSetShader(d11Shader->getPixelShader(), nullptr, 0);
		}

	}

	void D3D11Renderer::_clearShader(ShaderType st)
	{
		//in d11 ,cleat means we use fix-pipeline simulator
	}

	void D3D11Renderer::_setShaderParams(ShaderType type,const ShaderParamsPtr& params)
	{
		ID3D11Buffer* pBuffer = nullptr;
		if(type == ST_VERTEX_SHADER && mBoundVertexShader)
		{
			pBuffer = mBoundVertexShader->getConstBuffer(params);
			mpDeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
		}
		else if(type == ST_PIXEL_SHADER && mBoundPixelShader)
		{
			pBuffer = mBoundPixelShader->getConstBuffer(params);
			mpDeviceContext->PSGetConstantBuffers(0, 1, &pBuffer);
		}
		else if (type == ST_GEOMETRY_SHADER)
		{
			//todo, geometry shader
		}
	}

	void D3D11Renderer::_setViewMatrix(const Matrix4& view)
	{
		//todo,simulate fix-pipeline,set view
	}

	void D3D11Renderer::_setProjMatrix(const Matrix4& proj)
	{
		//todo,simulate fix-pipeline,set proj
	}

	void D3D11Renderer::_setWorldMatrix(const Matrix4& world)
	{
		//todo,simulate fix-pipeline,set world
	}


}