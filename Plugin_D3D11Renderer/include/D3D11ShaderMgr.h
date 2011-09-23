#ifndef __TITAN_D3D11HLSLShader_MGR__HH
#define __TITAN_D3D11HLSLShader_MGR__HH

#include "D3D11Prerequisites.h"
#include "TiShaderMgr.h"

namespace Titan
{
	class _D3D11DllExport D3D11ShaderMgr : public ShaderMgr
	{
	public:
		D3D11ShaderMgr();

		~D3D11ShaderMgr();

		Resource*	createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams);

	protected:

		Resource* createImpl(const String& name, ResourceHandle id, const String& group, ShaderType type);
	};
}

#endif