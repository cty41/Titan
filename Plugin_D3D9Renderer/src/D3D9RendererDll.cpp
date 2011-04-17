#include "TiRoot.h"
#include "D3D9Plugin.h"

namespace Titan
{
	D3D9Plugin* plugin;

	extern "C" void _D3D9DllExport dllStartPlugin(void) throw()
	{
		plugin = TITAN_NEW D3D9Plugin();
		Root::getSingleton().installPlugin(plugin);
	}

	extern "C" void _D3D9DllExport dllStopPlugin(void)
	{
		Root::getSingleton().uninstallPlugin(plugin);
		delete plugin;
	}
}