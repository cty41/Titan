#ifndef _D3D11PREQUISITES_HH
#define _D3D11PREQUISITES_HH

#include "TiPrerequisites.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#endif


#include <d3d11.h>
#include <D3Dcompiler.h>
#include <d3dx11.h>
#include <D3D11Shader.h>
#include <DxErr.h>

namespace Titan
{
	class D3D11RenderWindow;
	class D3D11Renderer;
	class D3D11Plugin;
	class D3D11IndexBuffer;
	class D3D11VertexBuffer;
	class D3D11VertexDecl;
	class D3D11HardwareBufferMgr;
	class D3D11Texture;
	class D3D11TextureMgr;
	class D3D11HLSLShader;
	class D3D11ShaderMgr;
	class D3D11VertexShader;
	class D3D11PixelShader;
	class D3D11FixedPipelineSimulator;

#ifdef PLUGIN_D3D11RENDERER_EXPORTS
#	define _D3D11DllExport __declspec(dllexport)
#else
#	define _D3D11DllExport __declspec(dllimport)
#endif


#define TITAN_EXCEPT_API_D11(hr, msg) {String errMsg = DXGetErrorDescription(hr); TITAN_EXCEPT_API(msg + errMsg);}
}



#endif