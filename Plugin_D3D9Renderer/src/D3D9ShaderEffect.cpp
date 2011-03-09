#include "D3D9ShaderEffect.h"

namespace Titan
{
	D3D9ShaderEffect::D3D9ShaderEffect(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
		:ShaderEffect(mgr, name, id, group), mEffect(0)
	{
	}
	//-------------------------------------------------------------//
	D3D9ShaderEffect::~D3D9ShaderEffect()
	{
		SAFE_RELEASE(mEffect);
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::loadImpl()
	{
		
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::unloadImpl()
	{

	}
}