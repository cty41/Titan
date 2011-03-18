#ifndef _TITAN_SHADEREFFECTMGR__HH
#define _TITAN_SHADEREFFECTMGR__HH

#include "TitanPrerequisites.h"
#include "TitanResourceManager.h"
#include "Singleton.h"
#include "TitanShaderEffect.h"

namespace Titan
{
	class _DllExport ShaderEffectMgr : public Singleton<ShaderEffectMgr>, public ResourceMgr
	{
	public:
		ShaderEffectMgr();

		~ShaderEffectMgr();

		ShaderEffectPtr loadManually(const String& name, const String& group);

		

	protected:

	public:
		static ShaderEffectMgr*	getSingletonPtr();

		static ShaderEffectMgr&	getSingleton();
	};

}

#endif