#include "TitanStableHeader.h"
#include "RenderWindow.h"
#include "Root.h"


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
	//-------------------------------------------------------------//
	void RenderWindow::update()
	{
		RenderTarget::update();
	}
	//-------------------------------------------------------------//
	void RenderWindow::destroy()
	{
	}

}