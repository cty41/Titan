#ifndef __D3D9RENDERER__HH
#define __D3D9RENDERER__HH

#include "D3D9Prerequisites.h"
#include "Renderer.h"
#include "TitanCommon.h"
#include "TitanColor.h"
#include "Singleton.h"
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

		void					_setCullingMode(CullingMode cm);

		void					_setShadeMode(ShadeOptions so);

		void					_setLightEnable(bool enable);

		void					_setFillMode(PolygonMode pm);

		HRESULT					__setRenderState(D3DRENDERSTATETYPE state, DWORD value);

		HRESULT					__SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value);

		void					_beginFrame();

		void					_endFrame();

		void					_clearFrameBuffer(unsigned int buffers, 
			const Color& color = Color::Black, 
			float depth = 1.0f, unsigned short stencil = 0);

		void					setVertexDeclaration(VertexDeclaration* decl);

		void					setVertexBufferBinding(VertexBufferBinding* binding);

		void					_render(const RenderData& rd);
		
		VertexElementType		getColorVertexElementType() const;

		void					_convertProjMatrix(Matrix4& projSrc, Matrix4& projDst);

		void					_setViewMatrix(const Matrix4& view);

		void					_setProjMatrix(const Matrix4& proj);

		void					_setWorldMatrix(const Matrix4& world);

		LPDIRECT3DDEVICE9		__getD3D9Device() const { return mpD3dDevice; }

		static D3D9Renderer*	getSingletonPtr();

		static D3D9Renderer&	getSingleton();

	protected:
		D3D9HardwareBufferManager*		mHardwareBufferManager;
		D3D9TextureMgr*					mTextureMgr;			
	
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