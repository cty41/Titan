#ifndef __TITAN_RENDERER__HH
#define __TITAN_RENDERER__HH

#include "TitanPrerequisites.h"
#include "TitanCommon.h"
#include "RenderData.h"
#include "TitanColor.h"
#include "TitanBlendMode.h"
#include "TiTextureUnit.h"

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

		virtual void			_beginGeometryCount(void);
		/** Reports the number of tris rendered since the last _beginGeometryCount call. */
		virtual unsigned int	_getFaceCount(void) const;
		/** Reports the number of batches rendered since the last _beginGeometryCount call. */
		virtual unsigned int	_getBatchCount(void) const;
		/** Reports the number of vertices passed to the renderer since the last _beginGeometryCount call. */
		virtual unsigned int	_getVertexCount(void) const;

		virtual void			_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram);

		void		convertColor(const Color& col, uint32* pDest);
		
		virtual	void			_setViewMatrix(const Matrix4& view) = 0;

		virtual	void			_setProjMatrix(const Matrix4& proj) = 0;

		virtual	void			_setWorldMatrix(const Matrix4& proj) = 0;

		virtual void			_beginFrame() = 0;

		virtual void			_endFrame() = 0;

		virtual void			_setCullingMode(CullingMode cm) = 0;

		virtual void			_setShadeMode(ShadeOptions so) = 0;

		virtual void			_setLightEnable(bool enable) = 0;

		virtual void			_setFillMode(PolygonMode pm) = 0;

		virtual void			_setDepthCheck(bool state) = 0;

		virtual void			_setDepthWrite(bool state) = 0;

		virtual void			_setDepthFuntion(CompareFunction cf) = 0;

		virtual void			_setTextureUnit(size_t idx, TextureUnit* tu);

		virtual void			_setTexture(size_t stage, const TexturePtr& pTex) = 0;

		virtual void			_setTexAddressMode(uint stage, const TexAddressModeSets& tam) = 0;

		void			_setSamplerFilter(uint sampler, FilterOptions minFilter, FilterOptions magFilter, FilterOptions mipFilter);

		virtual void			_setSamplerFilter(uint sampler, FilterType type, FilterOptions fo) = 0;

		virtual void			_setTexCoordSet(uint stage, size_t texcoordSet) = 0;

		virtual void			_setSceneBlending( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op = SBO_ADD) = 0;

		virtual float			getMaxDepthValue() = 0;

		virtual float			getMinDepthValue() = 0;

		virtual void			resumeFixFunction() = 0;

		virtual float			getVerticalTexelOffset() = 0;

		virtual float			getHorizontalTexelOffset() = 0;





	protected:
		int						mHeight;
		int						mWidth;
		ConfigOptionMap			mConfigOptions;
		RenderWindow*			mWindow;
		String					mName;
		RenderTargetMap			mTargetMap;
		PriorityTargetMap		mPriorityTargetMap;
		bool					mWaitForVSync;

		size_t					mBatchCount;
		size_t					mFaceCount;
		size_t					mVertexCount;
	};
}
#endif