#ifndef __D3D11RENDERER__HH
#define __D3D11RENDERER__HH

#include "D3D11Prerequisites.h"
#include "TiRenderer.h"
#include "TiCommon.h"
#include "TiColor.h"
#include "TiSingleton.h"
#include "TiMatrix4.h"
#include "D3D11CacheStates.h"

namespace Titan
{
	class _D3D11DllExport D3D11Renderer : public Renderer, public Singleton<D3D11Renderer>
	{
	public:
		D3D11Renderer(HINSTANCE hInstance);

		~D3D11Renderer();

		RenderWindow*			initialize();

		void					destroy();

		void					_beginFrame();

		void					_render(const RenderData& rd);

		void					_endFrame();

		const String&			getName() const;

		RenderWindow*			createRenderWindow(const String& caption, uint width, uint height, bool fullscreen);

		virtual void			initRenderDevice();

		String					getErrorDiscription(long result) const;

		//todo, simulate fixed-pipeline function [Begin]
		void					_setViewMatrix(const Matrix4& view);

		void					_setWorldMatrix(const Matrix4& world);

		void					_setProjMatrix(const Matrix4& proj);

		void					_setShadeMode(ShadeOptions so){}

		void					_setLightEnable(bool enable){}
		//todo, simulate fixed-pipeline function [End]

		void					_setTexture(size_t stage, const TexturePtr& pTex);

		void					_setViewport(Viewport* vp);

		void					_setCullingMode(CullingMode cm);

		void					_setPolygonMode(PolygonMode pm);

		void					_clearFrameBuffer(unsigned int buffers, 
			const Color& color = Color::Black, 
			float depth = 1.0f, unsigned short stencil = 0);

		void					setVertexDeclaration(VertexDeclaration* decl);

		void					setVertexBufferBinding(VertexBufferBinding* binding);

		void					_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram);

		void					_setDepthCheck(bool state);

		void					_setDepthWrite(bool state);

		void					_setDepthFuntion(CompareFunction cf);

		void					_setSamplerFilter(uint sampler, FilterType type, FilterOptions fo);

		void					_setTexCoordSet(uint stage, size_t texcoordSet);

		void					_setTexAddressMode(uint stage, const TexAddressModeSets& tam);

		void					_setSceneBlending( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op);

		void					_setShader(Shader* shader);

		void					_clearShader(ShaderType st);

		void					_setShaderParams(ShaderType type,const ShaderParamsPtr& params);

		float					getMinDepthValue() { return 0.0f;}

		// Range [0.0f, 1.0f]
		// D3D inverts even identity view matrices, so maximum INPUT is -1.0
		float					getMaxDepthValue(){ return -1.0f;}

		// D3D considers the origin to be in the center of a pixel, so we need to offset -0.5f
		float					getHorizontalTexelOffset() { return -0.5f;}

		float					getVerticalTexelOffset() { return -0.5f; }

		VertexElementType		getColorVertexElementType(void) const{ return VET_COLOR_ARGB; }

		void					_setTextureMatrix(size_t unit, const Matrix4& xform);

		ID3D11Device*			__getD3D11Device() const { return mpD3dDevice; }

		ID3D11DeviceContext*	__getD3D11DeviceContext() const { return mpDeviceContext; }

		ID3D11SamplerState*		_getCacheSamplerState();

		ID3D11BlendState*		_getCacheBlendState();

		ID3D11DepthStencilState*	_getCacheDepthStencilState();

		ID3D11RasterizerState*		_getCacheRasterizerState();

		static D3D11Renderer*	getSingletonPtr();

		static D3D11Renderer&	getSingleton();

	public:
		typedef map<uint32, ID3D11RasterizerState*> RasterizerStateMap;
		typedef map<uint32, ID3D11SamplerState*> SamplerStateMap;
		typedef map<uint32, ID3D11BlendState*> BlendStateMap;
		typedef map<uint32, ID3D11DepthStencilState*> DepthStencilStateMap;
		 
	protected:
		D3D11HardwareBufferMgr*		mHardwareBufferMgr;
		D3D11TextureMgr*			mTextureMgr;			
		D3D11ShaderMgr*				mShaderMgr;

		HINSTANCE				mhInstance;
		HWND					mhWnd;

		DXGI_SWAP_CHAIN_DESC	mDesc;
		ID3D11Device*			mpD3dDevice;
		ID3D11DeviceContext*	mpDeviceContext;
		IDXGISwapChain*			mSwapChain;
		D3D_DRIVER_TYPE			mDriverType;
		D3D_FEATURE_LEVEL		mFeatureLevel;
		ID3D11RenderTargetView*     mRenderTargetView;
		ID3D11Texture2D*            mDepthStencilTex;
		ID3D11DepthStencilView*     mDepthStencilView;

		size_t					mLastVertexSourceCount;

		//desc cache
		D3D11_BLEND_DESC		mBlendDesc;
		D3D11_SAMPLER_DESC		mSamplerDesc;
		D3D11_RASTERIZER_DESC	mRasterizerDesc;
		D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
		UINT mStencilRef;

		//manage render state objects in d11
		RasterizerStateMap		mRasterizerStateMap;
		SamplerStateMap			mSamplerStateMap;
		BlendStateMap			mBlendStateMap;
		DepthStencilStateMap	mDepthStencilStateMap;

		D3D11HLSLShader*		mBoundVertexShader;
		D3D11HLSLShader*		mBoundPixelShader;

		//cache draw primitive type
		OperationType			mCachePrimitiveType;
	};
}
#endif