#include "TitanStableHeader.h"
#include "D3D9Renderer.h"
#include "TiLogMgr.h"
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

		/// move this code into father class
		//loadDefaultConfigOptions();

		mpD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if(!mpD3D9)
			TITAN_EXCEPT_API("Direct3DCreate9 failed!");
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
		mpD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &mD3dCaps);

		mHardwareBufferManager = TITAN_NEW D3D9HardwareBufferMgr();

		mTextureMgr = TITAN_NEW D3D9TextureMgr();

		mShaderMgr = TITAN_NEW D3D9ShaderMgr();

		return Renderer::initialize();
	}
	//-------------------------------------------------------------------------------//
	const String& D3D9Renderer::getName() const
	{
		return mName;
	}

	void D3D9Renderer::initRenderDevice()
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
			TITAN_EXCEPT_API("D3D9 CreateDevice Failed");
		}
	}
	//-------------------------------------------------------------------------------//
	RenderWindow* D3D9Renderer::createRenderWindow(const String& caption, uint width, uint height, bool fullscreen)
	{
		mWidth = width; mHeight = height;
		mWindow = TITAN_NEW D3D9RenderWindow(mhInstance);
		mWindow->create(caption, width, height, fullscreen);
		mTargetMap.insert(RenderTargetMap::value_type(caption, mWindow));

		D3D9RenderWindow* d9Window = static_cast<D3D9RenderWindow*>(mWindow);
		d9Window->buildD3D9PrensentParameters(&mD3dpp);
		initRenderDevice();
		d9Window->setD3D9Device(mpD3dDevice);
		addTargetToRender(mWindow);

		return mWindow;
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
			TITAN_EXCEPT_API( "Failed to set viewport.");
	}
	//------------------------------------------------------------------------------//
	void D3D9Renderer::_setLightEnable(bool enable)
	{
		if(FAILED(__setRenderState(D3DRS_LIGHTING, enable)))
			TITAN_EXCEPT_API( "D3D9 switch lights failed ");
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setShadeMode(ShadeOptions so)
	{
		if(FAILED(__setRenderState(D3DRS_SHADEMODE, D3D9Mappings::convertToD3D9(so))))
			TITAN_EXCEPT_API( "D3D9 set Shading mode failed ");
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setPolygonMode(PolygonMode pm)
	{
		if(FAILED(__setRenderState(D3DRS_FILLMODE, D3D9Mappings::convertToD3D9(pm))))
			TITAN_EXCEPT_API( "D3D9 set Polygon mode failed ");
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setCullingMode(CullingMode cm)
	{
		if(FAILED(__setRenderState(D3DRS_CULLMODE, D3D9Mappings::convertToD3D9(cm, false))))
			TITAN_EXCEPT_API( "D3D9 set Culling mode failed ");
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
			TITAN_EXCEPT_API( "D3D9 BeginScene failed");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_endFrame()
	{
		HRESULT hr;
		if(FAILED(hr = mpD3dDevice->EndScene()))
		{
			String errMsg = getErrorDiscription(hr);
			TITAN_EXCEPT_API( "D3D9 EndScene failed");
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
			TITAN_EXCEPT_API( "Error clearing frame buffer : " + msg);
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
			TITAN_EXCEPT_API( "Unable to set D3D9 vertex declaration");
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
					TITAN_EXCEPT_API( "Unable to reset unused D3D9 stream source");
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
				TITAN_EXCEPT_API( "Unable to set D3D9 stream source for buffer binding");
			}

		}

		for (size_t unused = source; unused < mLastVertexSourceCount; ++unused)
		{

			hr = mpD3dDevice->SetStreamSource(static_cast<UINT>(unused), NULL, 0, 0);
			if (FAILED(hr))
			{
				TITAN_EXCEPT_API( "Unable to reset unused D3D9 stream source");
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
		case OT_POINT_LIST:
			primType = D3DPT_POINTLIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount);
			break;

		case OT_LINE_LIST:
			primType = D3DPT_LINELIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) / 2;
			break;

		case OT_LINE_STRIP:
			primType = D3DPT_LINESTRIP;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 1;
			break;

		case OT_TRIANGLE_LIST:
			primType = D3DPT_TRIANGLELIST;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) / 3;
			break;

		case OT_TRIANGLE_STRIP:
			primType = D3DPT_TRIANGLESTRIP;
			primCount = static_cast<DWORD>(rd.useIndex ? rd.indexData->indexCount : rd.vertexData->vertexCount) - 2;
			break;

		case OT_TRIANGLE_FAN:
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
				TITAN_EXCEPT_API(
					"d3d9 set index buffer failed");
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
			TITAN_EXCEPT_API( "Failed to DrawPrimitive or DrawIndexedPrimitive: " + msg);
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
			TITAN_EXCEPT_API( "Failed to set view matrix" + msg);
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
			TITAN_EXCEPT_API( "Failed to set proj matrix" + msg);
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
			TITAN_EXCEPT_API( "Failed to set world matrix" + msg);
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
					TITAN_EXCEPT_API( "Failed to enable wbuffer because of " + msg);
				}
			}
			else
			{
				if (FAILED(hr = __setRenderState(D3DRS_ZENABLE, D3DZB_TRUE)))
				{
					String msg = DXGetErrorDescription(hr);
					TITAN_EXCEPT_API( "Failed to enable zbuffer because of " + msg);
				}
			}
		}
		else
		{
			if (FAILED(hr = __setRenderState(D3DRS_ZENABLE, D3DZB_FALSE)))
			{
				String msg = DXGetErrorDescription(hr);
				TITAN_EXCEPT_API( "Failed to disable zbuffer because of " + msg);
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
			TITAN_EXCEPT_API( "Failed to set depth write because of " + msg);
		}
	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setDepthFuntion(CompareFunction cf)
	{
		HRESULT hr;
		if(FAILED(hr = __setRenderState(D3DRS_ZFUNC, D3D9Mappings::convertToD3D9(cf))))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "Failed to set depth function because of " + msg);
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setTexture(size_t stage, const TexturePtr& pTex)
	{
		HRESULT hr;
		if(!pTex.isNull())
		{
			D3D9Texture* dTex = static_cast<D3D9Texture*>(pTex.getPointer());
			IDirect3DBaseTexture9*  iTex = dTex->getD3dTexture();
			hr = mpD3dDevice->SetTexture(stage, iTex);
			if(FAILED(hr))
			{
				String str = String("Unable to set texture '" + pTex->getName() + "' in D3D9");
				TITAN_EXCEPT_API( str);
			}
		}
	}
	void D3D9Renderer::_setSamplerFilter(uint sampler, FilterType type, FilterOptions fo)
	{
		HRESULT hr;
		if(FAILED(hr = __SetSamplerState(sampler, D3D9Mappings::convertToD3D9(type),D3D9Mappings::convertToD3D9(type, fo))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "Got error:" + errMsg + " when set Sampler Filter");
		}
	}	
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setTexAddressMode(uint stage, const TexAddressModeSets& tam)
	{
		HRESULT hr ;
		if(FAILED(hr = __SetSamplerState(stage, D3DSAMP_ADDRESSU, D3D9Mappings::convertToD3D9(tam.UTexAddrMode))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("Got error:" + errMsg + " when set U");
		}

		if(FAILED(hr = __SetSamplerState(stage, D3DSAMP_ADDRESSV, D3D9Mappings::convertToD3D9(tam.VTexAddrMode))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("Got error:" + errMsg + " when set V");
		}

		if(FAILED(hr = __SetSamplerState(stage, D3DSAMP_ADDRESSW, D3D9Mappings::convertToD3D9(tam.WTexAddrMode))))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("Got error:" + errMsg + " when set W");
		}

	}
	//----------------------------------------------------------------------------//
	void D3D9Renderer::_setTexCoordSet(uint stage, size_t texcoordSet)
	{
		HRESULT hr;
		if(FAILED(hr = __SetTexStageState(stage, D3DTSS_TEXCOORDINDEX, texcoordSet)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("Got error:" + errMsg + " when set TexCoordSet");
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9Renderer::_setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		HRESULT hr;
		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			if (FAILED(hr = __setRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
				TITAN_EXCEPT_API( "Failed to set alpha blending option" );
		}
		else
		{
			if (FAILED(hr = __setRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
				TITAN_EXCEPT_API( "Failed to set alpha blending option" );
			if (FAILED(hr = __setRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE)))
				TITAN_EXCEPT_API( "Failed to set separate alpha blending option" );
			if( FAILED( hr = __setRenderState( D3DRS_SRCBLEND, D3D9Mappings::convertToD3D9(sourceFactor) ) ) )
				TITAN_EXCEPT_API( "Failed to set source blend" );
			if( FAILED( hr = __setRenderState( D3DRS_DESTBLEND, D3D9Mappings::convertToD3D9(destFactor) ) ) )
				TITAN_EXCEPT_API( "Failed to set destination blend" );
		}

		if (FAILED(hr = __setRenderState(D3DRS_BLENDOP, D3D9Mappings::convertToD3D9(op))))
			TITAN_EXCEPT_API( "Failed to set scene blending operation option" );
		if (FAILED(hr = __setRenderState(D3DRS_BLENDOPALPHA, D3D9Mappings::convertToD3D9(op))))
			TITAN_EXCEPT_API( "Failed to set scene blending operation option");
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
					TITAN_EXCEPT_API("set vertex shader failed because of " + errMsg);
				}
				break;
			}
		case ST_PIXEL_SHADER:
			{
				hr = mpD3dDevice->SetPixelShader(static_cast<D3D9PixelShader*>(shader)->getPixelShader());
				if(FAILED(hr))
				{
					String errMsg = DXGetErrorDescription(hr);
					TITAN_EXCEPT_API("set pixel shader failed because of " + errMsg);
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
				TITAN_EXCEPT_API("clear vertex shader failed because of " + errMsg);
			}
			break;
		case ST_PIXEL_SHADER:
			if(hr = mpD3dDevice->SetPixelShader(NULL))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT_API("clear pixel shader failed because of " + errMsg);
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
					for (auto i = floatRegs->bufferMap.cbegin();i != floatRegs->bufferMap.cend();++i)
					{
						size_t regIndex = i->first;
						const float* pFloat = params->getFloatPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetVertexShaderConstantF(
							(UINT)regIndex, pFloat, (UINT)slotCount)))
						{
							TITAN_EXCEPT_API( "Unable to update vertex shader float params");
						}
					}
					break;
				}
			case ST_PIXEL_SHADER:
				{
					for (auto i = floatRegs->bufferMap.cbegin();i != floatRegs->bufferMap.cend();++i)
					{
						size_t regIndex = i->first;
						const float* pFloat = params->getFloatPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetPixelShaderConstantF(
							(UINT)regIndex, pFloat, (UINT)slotCount)))
						{
							TITAN_EXCEPT_API( "Unable to update pixel shader float params");
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
					for (auto i = intRegs->bufferMap.cbegin();i != intRegs->bufferMap.cend();++i)
					{
						size_t regIndex = i->first;
						const int* pInt = params->getIntPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetVertexShaderConstantI(
							(UINT)regIndex, pInt, (UINT)slotCount)))
						{
							TITAN_EXCEPT_API( "Unable to update vetex shader int params");
						}
					}
					break;
				}
			case ST_PIXEL_SHADER:
				{
					for (auto i = intRegs->bufferMap.cbegin();i != intRegs->bufferMap.cend();++i)
					{
						size_t regIndex = i->first;
						const int* pInt = params->getIntPtr(i->second.physicalIndex);
						size_t slotCount = i->second.currentSize / 4;
						assert (i->second.currentSize % 4 == 0 && "Should not have any "
							"elements less than 4 wide for D3D9");

						if (FAILED(hr = mpD3dDevice->SetPixelShaderConstantI(
							(UINT)regIndex, pInt, (UINT)slotCount)))
						{
							TITAN_EXCEPT_API("Unable to update pixel shader int params");
						}
					}
					break;
				}
			}
		}
	}
	//------------------------------------------------------------------------------//
	void D3D9Renderer::_setTextureMatrix(size_t unit, const Matrix4& xform)
	{
		//add later
	}

}