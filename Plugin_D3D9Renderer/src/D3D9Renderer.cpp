#include "TitanStableHeader.h"
#include "D3D9Renderer.h"
#include "ConsoleDebugger.h"
#include "D3D9RenderWindow.h"
#include "D3D9Mappings.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9HardwareBufferManager.h"
#include "D3D9IndexBuffer.h"
#include "D3D9VertexBuffer.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9TextureMgr.h"

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
		SAFE_DELETE(mTextureMgr);
		SAFE_DELETE(mHardwareBufferManager);
		SAFE_RELEASE(mpD3dDevice);
		SAFE_RELEASE(mpD3D9);
	}

	RenderWindow* D3D9Renderer::initialize()
	{
		HRESULT hr = 0;

		mpD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &mD3dCaps);

		mHardwareBufferManager = TITAN_NEW D3D9HardwareBufferManager();

		mTextureMgr = TITAN_NEW D3D9TextureMgr();

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
	//-------------------------------------------------------------//
	const String& D3D9Renderer::getName() const
	{
		return mName;
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	void D3D9Renderer::preRender()
	{
		_beginFrame();
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::render()
	{
		_clearFrameBuffer(FBT_COLOR | FBT_DEPTH, Color::Blue);
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::postRender()
	{
		_endFrame();
	}
	//-------------------------------------------------------------//
	String D3D9Renderer::getErrorDiscription(long result) const 
	{
		const String errMsg = DXGetErrorDescription(result);
		return errMsg;
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setLightEnable(bool enable)
	{
		if(FAILED(__setRenderState(D3DRS_LIGHTING, enable)))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 switch lights failed ", "void D3D9Renderer::_setLightEnable(bool enable)");
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setShadeMode(ShadeOptions so)
	{
		if(FAILED(__setRenderState(D3DRS_SHADEMODE, D3D9Mappings::convertToD3D9(so))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 set Shading mode failed ", "void D3D9Renderer::_setShadeMode(ShadeOptions so)");
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setFillMode(PolygonMode pm)
	{
		if(FAILED(__setRenderState(D3DRS_FILLMODE, D3D9Mappings::convertToD3D9(pm))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 set Polygon mode failed ", "void D3D9Renderer::_setFillMode(PolygonMode pm)");
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setCullingMode(CullingMode cm)
	{
		if(FAILED(__setRenderState(D3DRS_CULLMODE, D3D9Mappings::convertToD3D9(cm, false))))
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 set Culling mode failed ", "void D3D9Renderer::_setCullingMode(CullingMode cm)");
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	void D3D9Renderer::_beginFrame()
	{
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->BeginScene()))
		{
			String errMsg = getErrorDiscription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 BeginScene failed", "void D3D9Renderer::_beginFrame()");
		}
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_endFrame()
	{
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->EndScene()))
		{
			String errMsg = getErrorDiscription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "D3D9 EndScene failed", "void D3D9Renderer::_endFrame()");
		}
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	VertexElementType D3D9Renderer::getColorVertexElementType() const
	{
		return VET_COLOR_ARGB;
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	void D3D9Renderer::_convertProjMatrix(Matrix4& projSrc, Matrix4& projDst)
	{
		projDst = projSrc;

		projDst[2][0] = (projDst[2][0] + projDst[3][0]) / 2;
		projDst[2][1] = (projDst[2][1] + projDst[3][1]) / 2;
		projDst[2][2] = (projDst[2][2] + projDst[3][2]) / 2;
		projDst[2][3] = (projDst[2][3] + projDst[3][3]) / 2;

		//if (!forGpuProgram)
		{
			// Convert right-handed to left-handed
			projDst[0][2] = -projDst[0][2];
			projDst[1][2] = -projDst[1][2];
			projDst[2][2] = -projDst[2][2];
			projDst[3][2] = -projDst[3][2];
		}
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setViewMatrix(const Matrix4& view)
	{
		mViewMatrix = view;
		mViewMatrix[2][0] = -mViewMatrix[2][0];
		mViewMatrix[2][1] = -mViewMatrix[2][1];
		mViewMatrix[2][2] = -mViewMatrix[2][2];
		mViewMatrix[2][3] = -mViewMatrix[2][3];
		D3DXMATRIX viewMatrix = D3D9Mappings::makeD3DXMatrix(mViewMatrix);
		HRESULT hr;
		if(FAILED(mpD3dDevice->SetTransform(D3DTS_VIEW, &viewMatrix)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set view matrix" + msg, "D3D9Renderer::_setViewMatrix");
		}
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setProjMatrix(const Matrix4& proj)
	{
		D3DXMATRIX projMatrix = D3D9Mappings::makeD3DXMatrix(proj);
		HRESULT hr;
		if(FAILED(mpD3dDevice->SetTransform(D3DTS_PROJECTION, &projMatrix)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set proj matrix" + msg, "D3D9Renderer::_setProjMatrix");
		}
	}
	//-------------------------------------------------------------//
	void D3D9Renderer::_setWorldMatrix(const Matrix4& world)
	{
		D3DXMATRIX worldMatrix = D3D9Mappings::makeD3DXMatrix(world);
		HRESULT hr;
		if(FAILED(mpD3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Failed to set world matrix" + msg, "D3D9Renderer::_setWorldMatrix");
		}
	}


}