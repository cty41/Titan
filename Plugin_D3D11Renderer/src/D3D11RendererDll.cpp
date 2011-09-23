#include "TiRoot.h"
#include "D3D11Plugin.h"

namespace Titan
{
	D3D11Plugin* plugin;

	extern "C" void _D3D11DllExport dllStartPlugin(void) throw()
	{
		plugin = TITAN_NEW D3D11Plugin();
		Root::getSingleton().installPlugin(plugin);
	}

	extern "C" void _D3D11DllExport dllStopPlugin(void)
	{
		Root::getSingleton().uninstallPlugin(plugin);
		TITAN_DELETE plugin;
	}
}