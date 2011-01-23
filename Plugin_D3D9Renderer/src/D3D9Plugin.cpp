#include "D3D9Plugin.h"
#include "Root.h"

namespace Titan
{
	const String cPluginName = "Direct3D9 Renderer";
	D3D9Plugin::D3D9Plugin()
		: mRenderer(0)
	{
	}
	//-------------------------------------------------------------//
	const String& D3D9Plugin::getName() const
	{
		return cPluginName;
	}
	//-------------------------------------------------------------//
	void D3D9Plugin::install()
	{
#if DEBUG
		HINSTANCE hInst = GetModuleHandle("Plugin_D3D9Render_d.dll");
#else
		HINSTANCE hInst = GetModuleHandle("Plugin_D3D9Render.dll");
#endif

		mRenderer = TITAN_NEW D3D9Renderer(hInst);

		Root::getSingleton().addRenderer(mRenderer);
	}
	//-------------------------------------------------------------//
	void D3D9Plugin::initialise()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void D3D9Plugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void D3D9Plugin::uninstall()
	{
		delete mRenderer;
		mRenderer = 0;
	}
}