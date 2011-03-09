#ifndef _TITAN_SHADEREFFECTMGR_HH
#define _TITAN_SHADEREFFECTMGR_HH

#include "D3D9Prerequisites.h"
#include "ShaderEffectMgr.h"

namespace Titan
{
	class _D3D9DllExport D3D9ShaderEffectMgr : public ShaderEffectMgr
	{
	public:
		D3D9ShaderEffectMgr();

		~D3D9ShaderEffectMgr();


	protected:
		Resource* createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams);
	};

}

#endif