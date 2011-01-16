#ifndef __TITAN_RENDERTARGET__HH
#define __TITAN_RENDERTARGET__HH

#include "TitanPrerequisites.h"
#include "TitanCommon.h"

namespace Titan
{
	enum RenderTargetType
	{
		RTT_WINDOW = 0,
		RTT_TEX = 1
	};
	class _DllExport RenderTarget : public GeneralAlloc
	{
	public:
		RenderTarget();

		virtual ~RenderTarget();

		virtual void create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams = 0) = 0;

		virtual void update();

		virtual void destroy() = 0;

		virtual void swapBuffer(){};

		virtual Viewport* addViewport(Camera* cam, int ZOrder = 0, float left = 0.0f, float top = 0.0f ,
			float width = 1.0f, float height = 1.0f);

		virtual Viewport* getViewport(unsigned short index);

		virtual void		removeViewport(int ZOrder);

		virtual void		removeAllViewports();

		virtual unsigned short getNumViewports(void) const;

		virtual RenderTargetType	getType() const = 0;

		virtual void				setPriority(uchar priority){ mPriority = priority; }

		uchar						getPriority() const { return mPriority; }

		uint	getHeight() const { return mHeight; }

		uint	getWidth() const { return mWidth; }

		virtual void _beginUpdate();

		virtual void _endUpdate();

		virtual void getCustomParams(const String& name, void* pDst);

	protected:
		RenderTargetType		mType;
		uchar					mPriority;

		unsigned int			mWidth;
		unsigned int			mHeight;

		typedef map<int, Viewport*>::type	ViewportList;
		ViewportList			mViewportList;

		virtual void _updateImpl();

		void	_updateAutoUpdateViewports();

		void	_updateViewport(Viewport* vp);
	};
}
#endif