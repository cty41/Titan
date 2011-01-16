#ifndef _D3D9PREQUISITES_HH
#define _D3D9PREQUISITES_HH

#include "TitanPrerequisites.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

namespace Titan
{
	class D3D9RenderWindow;
	class D3D9Renderer;
	class D3D9Plugin;
	class D3D9VertexBuffer;
	class D3D9VertexDeclaration;
	class D3D9HardwareBufferManager;
}

#ifdef PLUGIN_D3D9RENDERER_EXPORTS
#	define _D3D9DllExport __declspec(dllexport)
#else
#	define _D3D9DllExport __declspec(dllimport)
#endif

#endif