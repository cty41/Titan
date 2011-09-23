#ifndef _TITAN_D3D11PLUGIN_HH
#define _TITAN_D3D11PLUGIN_HH

#include "D3D11Prerequisites.h"
#include "TiPlugin.h"
#include "D3D11Renderer.h"

namespace Titan
{
	class _D3D11DllExport D3D11Plugin : public Plugin
	{
	public:
		D3D11Plugin();

		const String& getName() const;

		void install();

		void initialise();

		void shutdown();

		void uninstall();

	protected:
		D3D11Renderer*	mRenderer;
	};
}

#endif