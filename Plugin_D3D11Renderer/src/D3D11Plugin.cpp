#include "D3D11Plugin.h"
#include "D3D11Renderer.h"
#include "TiRoot.h"

namespace Titan
{
	const String cPluginName = "Direct3D9 Renderer";
	D3D11Plugin::D3D11Plugin()
		: mRenderer(0)
	{
	}
	//-------------------------------------------------------------------------------//
	const String& D3D11Plugin::getName() const
	{
		return cPluginName;
	}
	//-------------------------------------------------------------------------------//
	void D3D11Plugin::install()
	{
#if DEBUG
		HINSTANCE hInst = GetModuleHandle("Plugin_D3D11Render_d.dll");
#else
		HINSTANCE hInst = GetModuleHandle("Plugin_D3D19Render.dll");
#endif

		mRenderer = TITAN_NEW D3D11Renderer(hInst);

		Root::getSingleton().addRenderer(mRenderer);
	}
	//-------------------------------------------------------------------------------//
	void D3D11Plugin::initialise()
	{
		// nothing to do
	}
	//-------------------------------------------------------------------------------//
	void D3D11Plugin::shutdown()
	{
		// nothing to do
	}
	//-------------------------------------------------------------------------------//
	void D3D11Plugin::uninstall()
	{
		SAFE_DELETE(mRenderer);
	}
}