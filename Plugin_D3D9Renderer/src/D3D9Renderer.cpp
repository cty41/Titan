#include "TitanStableHeader.h"
#include "D3D9Renderer.h"
#include "TiConsoleDebugger.h"
#include "D3D9RenderWindow.h"
#include "D3D9Mappings.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9HardwareBufferMgr.h"
#include "D3D9IndexBuffer.h"
#include "D3D9VertexBuffer.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9TextureMgr.h"
#include "D3D9Texture.h"
#include "D3D9Shader.h"
#include "D3D9ShaderMgr.h"
#include "TiViewport.h"

namespace Titan
{
	template<> D3D9Renderer* Singleton<D3D9Renderer>::ms_Singleton = 0;

	D3D9Renderer* D3D9Renderer::getSingletonPtr()
	{
		return ms_Singleton;
	}
	D3D9Renderer& D3D9Renderer::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}


	D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)
		: Renderer(),mhInstance(0),
		mhWnd(0), mpD3D9(0), mpD3dDevice(0),
		mHardwareBufferManager(0), mTextureMgr(0),
		mLastVertexSourceCount(0)
	{
		mName = "Direct3D9 Renderer";

		ZeroMemory( &mD3dpp, sizeof( mD3dpp ) );

		loadDefaultConfigOptions();

		mpD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if(!mpD3D9)
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
				"Direct3DCreate9 failed!", "D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)");
	}

	D3D9Renderer::~D3D9Renderer()
	{
		destroy();
	}

	void D3D9Renderer::destroy()
	{
		SAFE_DELETE(mShaderMgr);
		SAFE_DELETE(mTextureMgr);
		SAFE_DELETE(mHardwareBufferManager);
		SAFE_RELEASE(mpD3dDevice);
		SAFE_RELEASE(mpD3D9);
	}

	RenderWindow* D3D9Renderer::initialize()
	{
		HRESULT hr = 0;

		mpD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &mD3dCaps);

		mHardwareBufferManager = TITAN_NEW D3D9HardwareBufferMgr();

		mTextureMgr = TITAN_NEW D3D9TextureMgr();

		mShaderMgr = TITAN_NEW D3D9ShaderMgr();

		ConfigOptionMap::iterator opt = mConfigOptions.find("Full Screen");
		if(opt == mConfigOptions.end())
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR, "can not find fullscreen option", "void D3D9Renderer::initialize(HWND hwnd)");
		bool fullScreen = opt->second.value == "Yes";

		opt = mConfigOptions.find("Resolution Mode");
		if(opt == mConfigOptions.end())
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR, "Can not find Resolution mode option", "void D3D9Renderer::initialize(HWND hwnd)");

		uint width, height;
		String::size_type widthEnd = opt->second.value.find(' ');
		// we know that the height starts 3 characters after the width and goes until the next space
		String::size_type heightEnd = opt->second.value.find(' ', widthEnd+3);
		// Now we can parse out the values
		width = StringConverter::parseInt(opt->second.value.substr(0, widthEnd));
		height = StringConverter::parseInt(opt->second.value.substr(widthEnd+3, heightEnd));

		String windowTitle;
		opt = mConfigOptions.find("Window Title");
		if(opt == mConfigOptions.end())
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR, "Can not find Window Title option", "void D3D9Renderer::initialize(HWND hwnd)");
		windowTitle = opt->second.value;

		mWidth = width;
		mHeight = height;
		createRenderWindow(windowTitle, width, height, fullScreen);

		return mWindow;
	}
	//-------------------------------------------------------------------------------//
	const String& D3D9Renderer::getName() const
	{
		return mName;
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::loadDefaultConfigOptions()
	{
		ConfigOption resolutionMode;
		resolutionMode.name = "Resolution Mode";
		resolutionMode.value = "800 x 600";

		ConfigOption fullScreen;
		fullScreen.name = "Full Screen";
		fullScreen.value = "No";

		ConfigOption windowTitle;
		windowTitle.name = "Window Title";
		windowTitle.value = "Titan";

		mConfigOptions[resolutionMode.name] = resolutionMode;
		mConfigOptions[fullScreen.name] = fullScreen;
		mConfigOptions[windowTitle.name] = windowTitle;
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::createD3D9Device()
	{
		int vp = 0;
		if( (mD3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0)
		{
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
		else															
			vp = D3DCREATE_MIXED_VERTEXPROCESSING;

		if(FAILED(mpD3D9->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			mhWnd,
			vp,
			&mD3dpp,
			&mpD3dDevice)))
		{
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,"D3D9 CreateDevice Failed", "void D3D9Renderer::initialize()");
		}
	}
	//-------------------------------------------------------------------------------//
	RenderWindow* D3D9Renderer::createRenderWindow(const String& caption, uint width, uint height, bool fullscreen)
	{
		mWindow = TITAN_NEW D3D9RenderWindow(mhInstance);
		mWindow->create(caption, width, height, fullscreen);
		mTargetMap.insert(RenderTargetMap::value_type(caption, mWindow));

		static_cast<D3D9RenderWindow*>(mWindow)->buildD3D9PrensentParameters(&mD3dpp);
		createD3D9Device();
		static_cast<D3D9RenderWindow*>(mWindow)->setD3D9Device(mpD3dDevice);
		addTargetToRender(mWindow);

		return mWindow;
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::preRender()
	{
		_beginFrame();
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::render()
	{
		_clearFrameBuffer(FBT_COLOR | FBT_DEPTH, Color::Blue);
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::postRender()
	{
		_endFrame();
	}
	//-------------------------------------------------------------------------------//
	String D3D9Renderer::getErrorDiscription(long result) const 
	{
		const String errMsg = DXGetErrorDescription(result);
		return errMsg;
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setViewport(Viewport* vp)
	{
		D3DVIEWPORT9 d3dvp;
		HRESULT hr;
		// set viewport dimensions
		d3dvp.X = vp->getActualLeft();
		d3dvp.Y = vp->getActualTop();
		d3dvp.Width = vp->getActualWidth();
		d3dvp.Height = vp->getActualHeight();
		d3dvp.MinZ = 0.0f;
		d3dvp.MaxZ = 1.0f;

		if( FAILED( hr = mpD3dDevice->SetViewport( &d3dvp ) ) )
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set viewport.", "D3D9Renderer::_setViewport" );
	}
	//------------------------------------------------------------------------------//
	void D3D9Renderer::_setLightEnable(bool enable)
	{
		if(FAILED(__setRenderState(D3DRS_LIGHTING, enable)))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 switch lights failed ", "void D3D9Renderer::_setLightEnable(bool enable)");
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setShadeMode(ShadeOptions so)
	{
		if(FAILED(__setRenderState(D3DRS_SHADEMODE, D3D9Mappings::convertToD3D9(so))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 set Shading mode failed ", "void D3D9Renderer::_setShadeMode(ShadeOptions so)");
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setFillMode(PolygonMode pm)
	{
		if(FAILED(__setRenderState(D3DRS_FILLMODE, D3D9Mappings::convertToD3D9(pm))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 set Polygon mode failed ", "void D3D9Renderer::_setFillMode(PolygonMode pm)");
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setCullingMode(CullingMode cm)
	{
		if(FAILED(__setRenderState(D3DRS_CULLMODE, D3D9Mappings::convertToD3D9(cm, false))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 set Culling mode failed ", "void D3D9Renderer::_setCullingMode(CullingMode cm)");
	}
	//-------------------------------------------------------------------------------//
	HRESULT D3D9Renderer::__setRenderState(D3DRENDERSTATETYPE state, DWORD value)
	{
		HRESULT hr;
		DWORD oldValue;
		if(FAILED(hr = mpD3dDevice->GetRenderState(state, &oldValue)))
			return hr;
		if(value != oldValue)
		{
			if(FAILED(hr = mpD3dDevice->SetRenderState(state, value)))
				return hr;
		}
		return S_OK;
	}
	//-------------------------------------------------------------------------------//
	HRESULT D3D9Renderer::__SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value)
	{
		HRESULT hr;
		DWORD oldValue;
		if(FAILED(hr = mpD3dDevice->GetSamplerState(sampler, type, &oldValue)))
			return hr;
		if(value != oldValue)
		{
			if(FAILED(hr = mpD3dDevice->SetSamplerState(sampler, type, value)))
				return hr;
		}
		return S_OK;
	}
	//-------------------------------------------------------------------------------//
	HRESULT D3D9Renderer::__SetTexStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE stageType, DWORD value)
	{
		HRESULT hr;
		DWORD oldValue;
		if(FAILED(hr = mpD3dDevice->GetTextureStageState(stage, stageType, &oldValue)))
			return hr;
		
		if(value != oldValue)
		{
			if(FAILED(hr = mpD3dDevice->SetTextureStageState(stage, stageType, value)))
				return hr;
		}
		return S_OK;

	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_beginFrame()
	{
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->BeginScene()))
		{
			String errMsg = getErrorDiscription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 BeginScene failed", "void D3D9Renderer::_beginFrame()");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_endFrame()
	{
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->EndScene()))
		{
			String errMsg = getErrorDiscription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 EndScene failed", "void D3D9Renderer::_endFrame()");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_clearFrameBuffer(unsigned int buffers, const Color& color /* = Color::Black */, float depth /* = 1.0f */, unsigned short stencil /* = 0 */)
	{
		DWORD flags = 0;
		if (buffers & FBT_COLOR)
		{
			flags |= D3DCLEAR_TARGET;
		}
		if (buffers & FBT_DEPTH)
		{
			flags |= D3DCLEAR_ZBUFFER;
		}
		// Only try to clear the stencil buffer if supported
#if 0
		if (buffers & FBT_STENCIL && mCurrentCapabilities->hasCapability(RSC_HWSTENCIL))
		{
			flags |= D3DCLEAR_STENCIL;
		}
#endif
		HRESULT hr;
		if( FAILED( hr = mpD3dDevice->Clear( 
			0, 
			NULL, 
			flags,
			color.getAsARGB(), 
			depth, 
			stencil ) ) )
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Error clearing frame buffer : " 
				+ msg, "void D3D9Renderer::_clearFrameBuffer" );
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::setVertexDeclaration(VertexDeclaration* decl)
	{
		HRESULT hr;

		D3D9VertexDeclaration* d3ddecl = 
			static_cast<D3D9VertexDeclaration*>(decl);

		if (FAILED(hr = mpD3dDevice->SetVertexDeclaration(d3ddecl->getD3D9VertexDeclaration())))
		{
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Unable to set D3D9 vertex declaration", 
				"D3D9RenderSystem::setVertexDeclaration");
		}

	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::setVertexBufferBinding(VertexBufferBinding* binding)
	{
		HRESULT hr;

		const VertexBufferBinding::VertexBufferBindingMap& binds = binding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend;
		size_t source = 0;
		iend = binds.end();
		for (i = binds.begin(); i != iend; ++i, ++source)
		{
			// Unbind gap sources
			for ( ; source < i->first; ++source)
			{
				hr = mpD3dDevice->SetStreamSource(static_cast<UINT>(source), NULL, 0, 0);
				if (FAILED(hr))
				{
					TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Unable to reset unused D3D9 stream source", 
						"D3D9Renderer::setVertexBufferBinding");
				}
			}

			D3D9VertexBuffer* d3d9buf = 
				static_cast<D3D9VertexBuffer*>(i->second.get());
			hr = mpD3dDevice->SetStreamSource(
				static_cast<UINT>(source),
				d3d9buf->getD3D9VertexBuffer(),
				0, // no stream offset, this is handled in _render instead
				static_cast<UINT>(d3d9buf->getVertexSize()) // stride
				);
			if (FAILED(hr))
			{
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Unable to set D3D9 stream source for buffer binding", 
					"D3D9Renderer::setVertexBufferBinding");
			}

		}

		for (size_t unused = source; unused < mLastVertexSourceCount; ++unused)
		{

			hr = mpD3dDevice->SetStreamSource(static_cast<UINT>(unused), NULL, 0, 0);
			if (FAILED(hr))
			{
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Unable to reset unused D3D9 stream source", 
					"D3D9Renderer::setVertexBufferBinding");
			}

		}
		mLastVertexSourceCount = source;
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_render(const RenderData& rd)
	{
		if (rd.vertexData->vertexCount == 0)
			return;

		Renderer::_render(rd);

		setVertexDeclaration(rd.vertexData->vertexDecl);
		setVertexBufferBinding(rd.vertexData->vertexBufferBinding);

		// Determine rendering operation
		D3DPRIMITIVETYPE primType = D3DPT_TRIANGLELIST;
		DWORD primCount = 0;
		switch( rd.operationType )
		{
		case RenderData::OT_POINT_LIST:
			primType = D3DPT_POINTLIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount);
			break;

		case RenderData::OT_LINE_LIST:
			primType = D3DPT_LINELIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) / 2;
			break;

		case RenderData::OT_LINE_STRIP:
			primType = D3DPT_LINESTRIP;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 1;
			break;

		case RenderData::OT_TRIANGLE_LIST:
			primType = D3DPT_TRIANGLELIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) / 3;
			break;

		case RenderData::OT_TRIANGLE_STRIP:
			primType = D3DPT_TRIANGLESTRIP;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 2;
			break;

		case RenderData::OT_TRIANGLE_FAN:
			primType = D3DPT_TRIANGLEFAN;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 2;
			break;
		}

		if (!primCount)
			return;

		HRESULT hr;
		if(rd.useIndex)
		{
			D3D9IndexBuffer* idxBuf = 
				static_cast<D3D9IndexBuffer*>(rd.indexData->indexBuffer.get());

			hr = mpD3dDevice->SetIndices( idxBuf->getD3D9IndexBuffer() );
			if(FAILED(hr))
			{
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"d3d9 set index buffer failed", "D3D9Renderer::_render");
			}

			hr = mpD3dDevice->DrawIndexedPrimitive(
				primType,
				static_cast<INT>(rd.vertexData->vertexStart), 
				0, // Min vertex index - assume we can go right down to 0 
				static_cast<UINT>(rd.vertexData->vertexCount), 
				static_cast<UINT>(rd.indexData->indexStart), 
				static_cast<UINT>(primCount)
				);
		}
		else
		{
			hr = mpD3dDevice->DrawPrimitive(
				primType, 
				static_cast<UINT>(rd.vertexData->vertexStart), 
				static_cast<UINT>(primCount));
		}

		if( FAILED( hr ) )
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to DrawPrimitive or DrawIndexedPrimitive: " + msg, "D3D9Renderer::_render" );
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram)
	{
		projDst = projSrc;

		projDst[2][0] = (projDst[2][0] + projDst[3][0]) / 2;
		projDst[2][1] = (projDst[2][1] + projDst[3][1]) / 2;
		projDst[2][2] = (projDst[2][2] + projDst[3][2]) / 2;
		projDst[2][3] = (projDst[2][3] + projDst[3][3]) / 2;

		if (!forGpuProgram)
		{
			// Convert right-handed to left-handed
			projDst[0][2] = -projDst[0][2];
			projDst[1][2] = -projDst[1][2];
			projDst[2][2] = -projDst[2][2];
			projDst[3][2] = -projDst[3][2];
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setViewMatrix(const Matrix4& view)
	{
		mViewMatrix = view;
		mViewMatrix[2][0] = -mViewMatrix[2][0];
		mViewMatrix[2][1] = -mViewMatrix[2][1];
		mViewMatrix[2][2] = -mViewMatrix[2][2];
		mViewMatrix[2][3] = -mViewMatrix[2][3];
		D3DXMATRIX viewMatrix = D3D9Mappings::makeD3DXMatrix(mViewMatrix);
		HRESULT hr;
		if(FAILED( hr = mpD3dDevice->SetTransform(D3DTS_VIEW, &viewMatrix)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set view matrix" + msg, "D3D9Renderer::_setViewMatrix");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setProjMatrix(const Matrix4& proj)
	{
		D3DXMATRIX projMatrix = D3D9Mappings::makeD3DXMatrix(proj);
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->SetTransform(D3DTS_PROJECTION, &projMatrix)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set proj matrix" + msg, "D3D9Renderer::_setProjMatrix");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setWorldMatrix(const Matrix4& world)
	{
		D3DXMATRIX worldMatrix = D3D9Mappings::makeD3DXMatrix(world);
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set world matrix" + msg, "D3D9Renderer::_setWorldMatrix");
		}
	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setDepthCheck(bool state)
	{
		HRESULT hr;
		if(state)
		{
			if(mD3dCaps.RasterCaps & D3DPRASTERCAPS_WBUFFER )
			{
				if(FAILED(hr = __setRenderState(D3DRS_ZENABLE, D3DZB_USEW)))
				{
					String msg = DXGetErrorDescription(hr);
					TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to enable wbuffer because of " + msg, "D3D9Renderer::_setDepthCheck");
				}
			}
			else
			{
				if (FAILED(hr = __setRenderState(D3DRS_ZENABLE, D3DZB_TRUE)))
				{
					String msg = DXGetErrorDescription(hr);
					TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to enable zbuffer because of " + msg, "D3D9Renderer::_setDepthCheck");
				}
			}
		}
		else
		{
			if (FAILED(hr = __setRenderState(D3DRS_ZENABLE, D3DZB_FALSE)))
			{
				String msg = DXGetErrorDescription(hr);
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to disable zbuffer because of " + msg, "D3D9Renderer::_setDepthCheck");
			}
		}

	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setDepthWrite(bool state)
	{
		HRESULT hr ;
		if(FAILED(hr = __setRenderState(D3DRS_ZWRITEENABLE, state)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set depth write because of " + msg, "D3D9Renderer::_setDepthWrite");
		}
	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setDepthFuntion(CompareFunction cf)
	{
		HRESULT hr;
		if(FAILED(hr = __setRenderState(D3DRS_ZFUNC, D3D9Mappings::convertToD3D9(cf))))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set depth function because of " + msg, "D3D9Renderer::_setDepthFuntion");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setTexture(size_t stage, const TexturePtr& pTex)
	{
		HRESULT hr;
		if(!pTex.isNull())
		{
			D3D9Texture* dTex = static_cast<D3D9Texture*>(pTex.getPointer());
			IDirect3DBaseTexture9*  iTex = dTex->getBaseTexture();
			hr = mpD3dDevice->SetTexture(stage, iTex);
			if(FAILED(hr))
			{
				String str = String("Unable to set texture '" + pTex->getName() + "' in D3D9");
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
					str,
					"D3D9Renderer::_setTexture");
			}
		}
	}
	void D3D9Renderer::_setSamplerFilter(uint sampler, FilterType type, FilterOptions fo)
	{
		HRESULT hr;
		if(FAILED(hr = __SetSamplerState(sampler, D3D9Mappings::convertToD3D9(type),D3D9Mappings::convertToD3D9(type, fo))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"Got error:" + errMsg + " when set Sampler Filter",
				"D3D9Renderer::_setSamplerFilter");
		}
	}	
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setTexAddressMode(uint stage, const TexAddressModeSets& tam)
	{
		HRESULT hr ;
		if(FAILED(hr = __SetSamplerState(stage, D3DSAMP_ADDRESSU, D3D9Mappings::convertToD3D9(tam.UTexAddrMode))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"Got error:" + errMsg + " when set U",
				"D3D9Renderer::_setTexAddressMode");
		}

		if(FAILED(hr = __SetSamplerState(stage, D3DSAMP_ADDRESSV, D3D9Mappings::convertToD3D9(tam.VTexAddrMode))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"Got error:" + errMsg + " when set V",
				"D3D9Renderer::_setTexAddressMode");
		}

		if(FAILED(hr = __SetSamplerState(stage, D3DSAMP_ADDRESSW, D3D9Mappings::convertToD3D9(tam.WTexAddrMode))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"Got error:" + errMsg + " when set W",
				"D3D9Renderer::_setTexAddressMode");
		}

	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setTexCoordSet(uint stage, size_t texcoordSet)
	{
		HRESULT hr;
		if(FAILED(hr = __SetTexStageState(stage, D3DTSS_TEXCOORDINDEX, texcoordSet)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"Got error:" + errMsg + " when set TexCoordSet",
				"D3D9Renderer::_setTexCoordSet");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::resumeFixFunction()
	{
		mpD3dDevice->SetVertexShader(NULL);
		mpD3dDevice->SetPixelShader(NULL);
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		HRESULT hr;
		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			if (FAILED(hr = __setRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set alpha blending option", "D3D9Renderer::_setSceneBlending" );
		}
		else
		{
			if (FAILED(hr = __setRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set alpha blending option", "D3D9Renderer::_setSceneBlending" );
			if (FAILED(hr = __setRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE)))
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set separate alpha blending option", "D3D9Renderer::_setSceneBlending" );
			if( FAILED( hr = __setRenderState( D3DRS_SRCBLEND, D3D9Mappings::convertToD3D9(sourceFactor) ) ) )
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set source blend", "D3D9Renderer::_setSceneBlending" );
			if( FAILED( hr = __setRenderState( D3DRS_DESTBLEND, D3D9Mappings::convertToD3D9(destFactor) ) ) )
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set destination blend", "D3D9Renderer::_setSceneBlending" );
		}

		if (FAILED(hr = __setRenderState(D3DRS_BLENDOP, D3D9Mappings::convertToD3D9(op))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set scene blending operation option", "D3D9Renderer::_setSceneBlendingOperation" );
		if (FAILED(hr = __setRenderState(D3DRS_BLENDOPALPHA, D3D9Mappings::convertToD3D9(op))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set scene blending operation option", "D3D9Renderer::_setSceneBlendingOperation" );
	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setShader(Shader* shader)
	{
		HRESULT hr;
		switch(shader->getShaderType())
		{
		case ST_VERTEX_SHADER:
			{
				hr = mpD3dDevice->SetVertexShader(static_cast<D3D9VertexShader*>(shader)->getVertexShader());
				if(FAILED(hr))
				{
					String errMsg = DXGetErrorDescription(hr);
					TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
						"set vertex shader failed because of " + errMsg,
						"D3D9Renderer::_setShader");
				}
				break;
			}
		case ST_PIXEL_SHADER:
			{
				hr = mpD3dDevice->SetPixelShader(static_cast<D3D9PixelShader*>(shader)->getPixelShader());
				if(FAILED(hr))
				{
					String errMsg = DXGetErrorDescription(hr);
					TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
						"set pixel shader failed because of " + errMsg,
						"D3D9Renderer::_setShader");
				}
				break;
			}
		}


		// Make sure texcoord index is equal to stage value, As SDK Doc suggests:
		// "When rendering using vertex shaders, each stage's texture coordinate index must be set to its default value."
		// This solves such an errors when working with the Debug runtime -
		// "Direct3D9: (ERROR) :Stage 1 - Texture coordinate index in the stage must be equal to the stage index when programmable vertex pipeline is used".
		for (unsigned int nStage=0; nStage < 8; ++nStage)
			__SetTexStageState(nStage, D3DTSS_TEXCOORDINDEX, nStage);

		Renderer::_setShader(shader);
	}
	//------------------------------------------------------------------------------//
	void D3D9Renderer::_clearShader(ShaderType st)
	{
		HRESULT hr;
		switch(st)
		{
		case ST_VERTEX_SHADER:
			if(hr = mpD3dDevice->SetVertexShader(NULL))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
					"clear vertex shader failed because of " + errMsg,
					"D3D9Renderer::_clearShader");
			}
			break;
		case ST_PIXEL_SHADER:
			if(hr = mpD3dDevice->SetPixelShader(NULL))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
					"clear pixel shader failed because of " + errMsg,
					"D3D9Renderer::_clearShader");
			}
			break;
		}
		Renderer::_clearShader(st);
	}
	//------------------------------------------------------------------------------//
	void D3D9Renderer::_setShaderParams(ShaderType type,const ShaderParamsPtr& params)
	{
		HRESULT hr;
		if(params->hasFloatConstants())
		{
			ShaderRegisterBufferPtr floatRegs = params->getFloatRegisterBuffer();
			switch(type)
			{
			case ST_VERTEX_SHADER:
				{
					ShaderRegisterIndexUseMap::const_iterator i = floatRegs->bufferMap.begin();
					ShaderRegisterIndexUseMap::const_iterator itend = floatRegs->bufferMap.end();
					for (;i != itend;++i)
					{
						size_t regIndex = i->first;
						const float* pFloat = params->getFloatPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetVertexShaderConstantF(
							(UINT)regIndex, pFloat, (UINT)slotCount)))
						{
							TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
								"Unable to update vertex shader float params", 
								"D3D9Renderer::_setShaderParams");
						}
					}
					break;
				}
			case ST_PIXEL_SHADER:
				{
					ShaderRegisterIndexUseMap::const_iterator i = floatRegs->bufferMap.begin();
					ShaderRegisterIndexUseMap::const_iterator itend = floatRegs->bufferMap.end();
					for (;i != itend;++i)
					{
						size_t regIndex = i->first;
						const float* pFloat = params->getFloatPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetPixelShaderConstantF(
							(UINT)regIndex, pFloat, (UINT)slotCount)))
						{
							TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
								"Unable to update pixel shader float params", 
								"D3D9Renderer::_setShaderParams");
						}
					}
					break;
				}
			}
		}

		if(params->hasIntConstants())
		{
			ShaderRegisterBufferPtr intRegs = params->getIntRegisterBuffer();
			switch(type)
			{
			case ST_VERTEX_SHADER:
				{
					ShaderRegisterIndexUseMap::const_iterator i = intRegs->bufferMap.begin();
					ShaderRegisterIndexUseMap::const_iterator itend = intRegs->bufferMap.end();
					for (;i != itend;++i)
					{
						size_t regIndex = i->first;
						const int* pInt = params->getIntPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetVertexShaderConstantI(
							(UINT)regIndex, pInt, (UINT)slotCount)))
						{
							TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
								"Unable to update vetex shader int params", 
								"D3D9Renderer::_setShaderParams");
						}
					}
					break;
				}
			case ST_PIXEL_SHADER:
				{
					ShaderRegisterIndexUseMap::const_iterator i = intRegs->bufferMap.begin();
					ShaderRegisterIndexUseMap::const_iterator itend = intRegs->bufferMap.end();
					for (;i != itend;++i)
					{
						size_t regIndex = i->first;
						const int* pInt = params->getIntPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetPixelShaderConstantI(
							(UINT)regIndex, pInt, (UINT)slotCount)))
						{
							TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
								"Unable to update pixel shader int params", 
								"D3D9Renderer::_setShaderParams");
						}
					}
					break;
				}
			}
		}
	}


}