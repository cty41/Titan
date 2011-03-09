#include "D3D9ShaderEffectMgr.h"
#include "D3D9ShaderEffect.h"

namespace Titan
{
	D3D9ShaderEffectMgr::D3D9ShaderEffectMgr()
		:ShaderEffectMgr()
	{
	}
	//-------------------------------------------------------------//
	D3D9ShaderEffectMgr::~D3D9ShaderEffectMgr()
	{
	}
	//-------------------------------------------------------------//
	 Resource* D3D9ShaderEffectMgr::createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams)
	{
		Resource* effect = TITAN_NEW D3D9ShaderEffect(this,name, id, group);

		return effect;
	}
}