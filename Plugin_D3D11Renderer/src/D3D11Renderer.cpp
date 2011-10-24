#include "TitanStableHeader.h"
#include "TiViewport.h"

#include "D3D11Renderer.h"
#include "D3D11RenderWindow.h"
#include "D3D11Mappings.h"
#include "D3D11VertexDecl.h"
#include "D3D11HLSLShader.h"
#include "D3D11HardwareBufferMgr.h"
#include "D3D11ShaderMgr.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"

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
		mCachePrimitiveType(OT_TRIANGLE_LIST)
	{
		mName = "D3D11Renderer";

		ZeroMemory(&mBlendDesc, sizeof(D3D11_BLEND_DESC));
		mBlendDesc.AlphaToCoverageEnable = false;
		mBlendDesc.IndependentBlendEnable = false;
		ZeroMemory(&mSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		ZeroMemory(&mRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		ZeroMemory(&mDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
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
		//todo, add new mgrs here,texture...

		return Renderer::initialize();
	}

	void D3D11Renderer::destroy()
	{
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

		UINT numFeatureLevels = sizeof( featureLevels );

		for( uint driverTypeIndex = 0; driverTypeIndex < 2; driverTypeIndex++ )
		{
			mDriverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain( NULL, mDriverType, NULL, createDeviceFlags,
				featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &mDesc, &mSwapChain, &mpD3dDevice, &mFeatureLevel, &mpDeviceContext);
			if( SUCCEEDED( hr ) )
				break;
		}

		// Create a render target view
		ID3D11Texture2D* pBuffer;
		hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBuffer );
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API( "Swap Chain getBuffer failed");
		}

		hr = mpD3dDevice->CreateRenderTargetView( pBuffer, NULL, &mRenderTargetView );
		pBuffer->Release();
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API( "Create Render Target View failed");
		}

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = mWidth;
		descDepth.Height = mHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = mpD3dDevice->CreateTexture2D( &descDepth, NULL, &mDepthStencilTex );
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API("Create Depth texture failed");
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = mpD3dDevice->CreateDepthStencilView( mDepthStencilTex, &descDSV, &mDepthStencilView );
		if( FAILED( hr ) )
		{
			TITAN_EXCEPT_API("CreateDepthStencilView failed");
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
	}
	//-------------------------------------------------------------------------------//
	void D3D11Renderer::_setCullingMode(CullingMode cm)
	{
		mRasterizerDesc.CullMode = D3D11Mappings::convertToD3D11(cm);
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setDepthCheck(bool state)
	{
		mDepthStencilDesc.DepthEnable = state;
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setDepthWrite(bool state)
	{
		if(state)
		{
			mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		}
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setDepthFuntion(CompareFunction cf)
	{
		mDepthStencilDesc.DepthFunc = D3D11Mappings::convertToD3D11(cf);
	}
	//change later
	void D3D11Renderer::_setSamplerFilter(uint sampler, FilterType type, FilterOptions fo)
	{
		mSamplerDesc.Filter = D3D11Mappings::convertToD3D11(fo);
	}	
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setTexAddressMode(uint stage, const TexAddressModeSets& tam)
	{
		mSamplerDesc.AddressU = D3D11Mappings::convertToD3D11(tam.UTexAddrMode);
		mSamplerDesc.AddressV = D3D11Mappings::convertToD3D11(tam.VTexAddrMode);
		mSamplerDesc.AddressW = D3D11Mappings::convertToD3D11(tam.WTexAddrMode);
	}
	//----------------------------------------------------------------------------//
	void D3D11Renderer::_setTexCoordSet(uint stage, size_t texcoordSet)
	{
		//change later
		//HRESULT hr;
		//if(FAILED(hr = __SetTexStageState(stage, D3DTSS_TEXCOORDINDEX, texcoordSet)))
		//{
		//	String errMsg = DXGetErrorDescription(hr);
		//	TITAN_EXCEPT_API(
		//		"Got error:" + errMsg + " when set TexCoordSet",
		//		"D3D11Renderer::_setTexCoordSet");
		//}
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
		}
	}

	const String& D3D11Renderer::getName() const
	{
		return mName;
	}

	ID3D11RasterizerState* D3D11Renderer::_getCacheRasterizerState()
	{
#if DO_CACHE_D11STATES
		

#endif
		{
			ID3D11RasterizerState* samplerState = nullptr;

			mpD3dDevice->CreateRasterizerState(&mRasterizerDesc, &samplerState);

			return samplerState;
		}
	}

	ID3D11SamplerState* D3D11Renderer::_getCacheSamplerState()
	{
#if DO_CACHE_D11STATES


#endif
		{
			ID3D11SamplerState* samplerState = nullptr;
			mpD3dDevice->CreateSamplerState(&mSamplerDesc, &samplerState);
			return samplerState;
		}
	}

	ID3D11BlendState* D3D11Renderer::_getCacheBlendState()
	{
#if DO_CACHE_D11STATES


#endif
		{
			ID3D11BlendState* blendState = nullptr;
			mpD3dDevice->CreateBlendState(&mBlendDesc, &blendState);
			return blendState;
		}
	}

	ID3D11DepthStencilState* D3D11Renderer::_getCacheDepthStencilState()
	{
#if DO_CACHE_D11STATES


#endif
		{
			ID3D11DepthStencilState* dsState = nullptr;
			mpD3dDevice->CreateDepthStencilState(&mDepthStencilDesc, &dsState);
			return dsState;
		}
	}

	/// d3d11 render op, and call all real set state object here, is that right? -cty

	void D3D11Renderer::_render(const RenderData& rd)
	{
		if (rd.vertexData->vertexCount == 0)
			return;

		Renderer::_render(rd);

		mpDeviceContext->RSSetState(_getCacheRasterizerState());
		mpDeviceContext->OMSetDepthStencilState(_getCacheDepthStencilState(), mStencilRef);
		mpDeviceContext->OMSetBlendState(_getCacheBlendState(), 0, 0xffffffff);

		//todo, texture and sampler
		//mpD3dDevice->PSSetSamplers()

		DWORD primCount = 0;
		D3D11_PRIMITIVE_TOPOLOGY primType;

		switch( rd.operationType )
		{
		case OT_TRIANGLE_LIST:
			primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) / 3;
			break;
		case OT_TRIANGLE_STRIP:
			primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 2;
			break;
		}

		if (!primCount)
			return;

		if(mCachePrimitiveType != rd.operationType)
		{
			mpDeviceContext->IASetPrimitiveTopology(primType);
			mCachePrimitiveType = rd.operationType;
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

	}

	void D3D11Renderer::_beginFrame()
	{
		//null for d11
	}

	void D3D11Renderer::_endFrame()
	{
		//null for d11
	}

	void D3D11Renderer::_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram)
	{
		//d11 is right hand system
		projDst = projSrc;
	}

	void D3D11Renderer::_setTextureMatrix(size_t unit, const Matrix4& xform)
	{
		//to do
	}

	void D3D11Renderer::setVertexDeclaration(VertexDeclaration* decl)
	{
		D3D11VertexDecl* d11Decl = static_cast<D3D11VertexDecl*>(decl);
		//dx11 is different from d9 which need to bind vertex decl to shader manually to get better performance
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
		//fuck off
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

	void D3D11Renderer::_setTexture(size_t stage, const TexturePtr& pTex)
	{
		//todo,simulate fix-pipeline,set texture
	}
}