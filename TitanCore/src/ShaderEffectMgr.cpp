#include "TitanStableHeader.h"
#include "ShaderEffectMgr.h"
#include "TitanShaderEffect.h"

namespace Titan
{
	template<> ShaderEffectMgr* Singleton<ShaderEffectMgr>::ms_Singleton = 0;

	ShaderEffectMgr* ShaderEffectMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}

	ShaderEffectMgr& ShaderEffectMgr::getSingleton()
	{
		assert(ms_Singleton); return (*ms_Singleton);
	}

	ShaderEffectMgr::ShaderEffectMgr()
		:ResourceMgr()
	{
		mLoadOrder = 50;
	}
	//-------------------------------------------------------------------------------//
	ShaderEffectMgr::~ShaderEffectMgr()
	{

	}
	//-------------------------------------------------------------------------------//
	ShaderEffectPtr ShaderEffectMgr::loadManually(const String& name, const String& group)
	{
		ShaderEffectPtr shaderPtr = load(name, group);
		return shaderPtr;
	}
}