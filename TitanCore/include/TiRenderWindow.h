#ifndef	__RENDERWINDOW_HH
#define	__RENDERWINDOW_HH

#include "TiPrerequisites.h"
#include "TiRenderTarget.h"


namespace Titan
{
	class _DllExport RenderWindow : public RenderTarget
	{
	public:
		RenderWindow();

		virtual ~RenderWindow();

		virtual void create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams = 0);

		void update();

		void destroy();

		RenderTargetType	getType() const	{ return mType; }

	protected:
		bool					mIsWindowed;
		String					mWindowCaption;
	};
}
#endif