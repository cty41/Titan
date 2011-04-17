#include "TitanStableHeader.h"
#include "TiRenderWindow.h"
#include "TiRoot.h"


namespace Titan
{
	RenderWindow::RenderWindow()
		: mIsWindowed(true)
	{
		mType = RTT_WINDOW;
	}

	RenderWindow::~RenderWindow()
	{
		destroy();
	}
	
	void RenderWindow::create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams)
	{
	}
	//-------------------------------------------------------------------------------//
	void RenderWindow::update()
	{
		RenderTarget::update();
	}
	//-------------------------------------------------------------------------------//
	void RenderWindow::destroy()
	{
	}

}