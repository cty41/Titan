#ifndef __TITAN_D3D9SHADER_MGR__HH
#define __TITAN_D3D9SHADER_MGR__HH

#include "D3D9Prerequisites.h"
#include "TiShaderMgr.h"

namespace Titan
{
	class _D3D9DllExport D3D9ShaderMgr : public ShaderMgr
	{
	public:
		D3D9ShaderMgr();

		~D3D9ShaderMgr();

		Resource*	createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams);

	protected:

		Resource* createImpl(const String& name, ResourceHandle id, const String& group, ShaderType type);
	};
}

#endif