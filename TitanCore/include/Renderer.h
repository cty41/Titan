#ifndef __TITAN_RENDERER__HH
#define __TITAN_RENDERER__HH

#include "TitanPrerequisites.h"
#include "TitanCommon.h"
#include "RenderData.h"
#include "TitanColor.h"

namespace Titan
{
	typedef multimap<uchar, RenderTarget* >::type PriorityTargetMap;
	class _DllExport Renderer: public GeneralAlloc
	{
	public:
		Renderer();

		virtual ~Renderer();

		virtual RenderWindow*	initialize() = 0;

		virtual void			destroy() = 0;

		virtual void			preRender() = 0;

		virtual void			render() = 0;

		virtual void			postRender() = 0;

		virtual void			loadDefaultConfigOptions() = 0;

		virtual void			updateAllTargets();

		virtual void			swapAllTargetBuffers();

		RenderTarget*			getTarget(const String& name);

		void					deleteTarget(const String& name);
	
		void					addTargetToRender(RenderTarget* target);

		void					delTargetFromRender(RenderTarget* target);

		virtual RenderWindow*	createRenderWindow(const String& caption, uint width, uint height, bool fullscreen) = 0;

		const ConfigOptionMap&	getConfigOptionMap() const { return mConfigOptions; }

		void					setConfigOption(const String& first, const String& second);

		virtual const String&	getName() const = 0;

		bool					getVSync() const { return mWaitForVSync; }

		virtual VertexElementType getColorVertexElementType() const = 0;

		virtual void			setVertexDeclaration(VertexDeclaration* decl) = 0;

		virtual void			setVertexBufferBinding(VertexBufferBinding* binding) = 0;

		virtual void			_render(const RenderData& rd);

		virtual void			_clearFrameBuffer(unsigned int buffers, 
									const Color& color = Color::Black, 
									float depth = 1.0f, unsigned short stencil = 0) = 0;

		virtual void			_convertProjMatrix(Matrix4& projSrc, Matrix4& projDst);
		
		virtual	void			_setViewMatrix(const Matrix4& view) = 0;

		virtual	void			_setProjMatrix(const Matrix4& proj) = 0;

		virtual	void			_setWorldMatrix(const Matrix4& proj) = 0;

		virtual void			_beginFrame() = 0;

		virtual void			_endFrame() = 0;

		virtual void			_setCullingMode(CullingMode cm) = 0;

		virtual void			_setShadeMode(ShadeOptions so) = 0;

		virtual void			_setLightEnable(bool enable) = 0;

		virtual void			_setFillMode(PolygonMode pm) = 0;


	protected:
		int						mHeight;
		int						mWidth;
		ConfigOptionMap			mConfigOptions;
		RenderWindow*			mWindow;
		String					mName;
		RenderTargetMap			mTargetMap;
		PriorityTargetMap		mPriorityTargetMap;
		bool					mWaitForVSync;

		size_t mFaceCount;
		size_t mVertexCount;
	};
}
#endif