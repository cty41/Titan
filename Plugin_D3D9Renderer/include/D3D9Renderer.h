#ifndef __D3D9RENDERER__HH
#define __D3D9RENDERER__HH

#include "D3D9Prerequisites.h"
#include "TiRenderer.h"
#include "TitanCommon.h"
#include "TitanColor.h"
#include "TiSingleton.h"
#include "TitanMatrix4.h"

namespace Titan
{
	class _D3D9DllExport D3D9Renderer : public Renderer, public Singleton<D3D9Renderer>
	{
	public:
		D3D9Renderer(HINSTANCE hInstance);

		~D3D9Renderer();

		RenderWindow*			initialize();

		void					destroy();

		void					preRender();

		void					render();

		void					postRender();

		const String&			getName() const;

		void					loadDefaultConfigOptions();

		RenderWindow*			createRenderWindow(const String& caption, uint width, uint height, bool fullscreen);

		void					createD3D9Device();

		String					getErrorDiscription(long result) const;

		void					_setViewport(Viewport* vp);

		void					_setCullingMode(CullingMode cm);

		void					_setShadeMode(ShadeOptions so);

		void					_setLightEnable(bool enable);

		void					_setFillMode(PolygonMode pm);

		HRESULT					__setRenderState(D3DRENDERSTATETYPE state, DWORD value);

		HRESULT					__SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value);

		HRESULT					__SetTexStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE stageType, DWORD value);

		void					_beginFrame();

		void					_endFrame();

		void					_clearFrameBuffer(unsigned int buffers, 
			const Color& color = Color::Black, 
			float depth = 1.0f, unsigned short stencil = 0);

		void					setVertexDeclaration(VertexDeclaration* decl);

		void					setVertexBufferBinding(VertexBufferBinding* binding);

		void					_render(const RenderData& rd);

		void					_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram);

		void					_setViewMatrix(const Matrix4& view);

		void					_setProjMatrix(const Matrix4& proj);

		void					_setWorldMatrix(const Matrix4& world);

		void					_setDepthCheck(bool state);

		void					_setDepthWrite(bool state);

		void					_setDepthFuntion(CompareFunction cf);

		void					_setSamplerFilter(uint sampler, FilterType type, FilterOptions fo);

		void					_setTexCoordSet(uint stage, size_t texcoordSet);

		void					_setTexAddressMode(uint stage, const TexAddressModeSets& tam);

		void					_setTexture(size_t stage, const TexturePtr& pTex);

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

		void					resumeFixFunction();

		LPDIRECT3DDEVICE9		__getD3D9Device() const { return mpD3dDevice; }

		static D3D9Renderer*	getSingletonPtr();

		static D3D9Renderer&	getSingleton();

	protected:
		D3D9HardwareBufferMgr*		mHardwareBufferManager;
		D3D9TextureMgr*					mTextureMgr;			
		D3D9ShaderMgr*					mShaderMgr;


		D3DPRESENT_PARAMETERS	mD3dpp;
		HINSTANCE				mhInstance;
		HWND					mhWnd;
		LPDIRECT3D9				mpD3D9;
		LPDIRECT3DDEVICE9		mpD3dDevice;
		D3DCAPS9				mD3dCaps;
		Matrix4					mViewMatrix;
		Matrix4					mProjMatrix;
		size_t					mLastVertexSourceCount;

	};
}
#endif