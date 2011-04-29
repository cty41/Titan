#ifndef _TITAN_D3D9PLUGIN_HH
#define _TITAN_D3D9PLUGIN_HH

#include "D3D9Prerequisites.h"
#include "TiPlugin.h"
#include "D3D9Renderer.h"

namespace Titan
{
	class _D3D9DllExport D3D9Plugin : public Plugin
	{
	public:
		D3D9Plugin();

		const String& getName() const;

		void install();

		void initialise();

		void shutdown();

		void uninstall();

	protected:
		D3D9Renderer*	mRenderer;
	};
}

#endif