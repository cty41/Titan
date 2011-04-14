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
		enum StatFlags
		{
			SF_NONE           = 0,
			SF_FPS            = 1,
			SF_AVG_FPS        = 2,
			SF_BEST_FPS       = 4,
			SF_WORST_FPS      = 8,
			SF_TRIANGLE_COUNT = 16,
			SF_ALL            = 0xFFFF
		};

		struct FrameStats
		{
			float lastFPS;
			float avgFPS;
			float bestFPS;
			float worstFPS;
			unsigned long bestFrameTime;
			unsigned long worstFrameTime;
			size_t triangleCount;
			size_t batchCount;
		};

		enum FrameBuffer
		{
			FB_FRONT,
			FB_BACK,
			FB_AUTO
		};
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

		virtual void getStatistics(float& lastFPS, float& avgFPS,
            float& bestFPS, float& worstFPS) const;  // Access to stats

        virtual const FrameStats& getStatistics(void) const;

        /** Individual stats access - gets the number of frames per second (FPS) based on the last frame rendered.
        */
        virtual float getLastFPS() const;

        /** Individual stats access - gets the average frames per second (FPS) since call to Root::startRendering.
        */
        virtual float getAverageFPS() const;

        /** Individual stats access - gets the best frames per second (FPS) since call to Root::startRendering.
        */
        virtual float getBestFPS() const;

        /** Individual stats access - gets the worst frames per second (FPS) since call to Root::startRendering.
        */
        virtual float getWorstFPS() const;

        /** Individual stats access - gets the best frame time
        */
        virtual float getBestFrameTime() const;

        /** Individual stats access - gets the worst frame time
        */
        virtual float getWorstFrameTime() const;

        /** Resets saved frame-rate statistices.
        */
        virtual void resetStatistics(void);

		/** Gets the number of triangles rendered in the last update() call. */
		virtual size_t getTriangleCount(void) const;
		/** Gets the number of batches rendered in the last update() call. */
		virtual size_t getBatchCount(void) const;

	protected:
		RenderTargetType		mType;
		uchar					mPriority;

		unsigned int			mWidth;
		unsigned int			mHeight;

		typedef map<int, Viewport*>::type	ViewportList;
		ViewportList			mViewportList;

		// Stats
		FrameStats mStats;

		Timer* mTimer ;
		unsigned long mLastSecond;
		unsigned long mLastTime;
		size_t mFrameCount;

		void updateStats(void);

		virtual void _updateImpl();

		void	_updateAutoUpdateViewports(bool updateStats = true);

		void	_updateViewport(Viewport* vp, bool updateStats = true);
	};
}
#endif