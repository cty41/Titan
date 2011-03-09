#ifndef _TITAN_SHADEREFFECTMGR__HH
#define _TITAN_SHADEREFFECTMGR__HH

#include "TitanPrerequisites.h"
#include "TitanResourceManager.h"
#include "Singleton.h"

namespace Titan
{
	class _DllExport ShaderEffectMgr : public Singleton<ShaderEffectMgr>, public ResourceMgr
	{
	public:
		ShaderEffectMgr();

		~ShaderEffectMgr();

		

	protected:

	public:
		static ShaderEffectMgr*	getSingletonPtr();

		static ShaderEffectMgr&	getSingleton();
	};

}

#endif