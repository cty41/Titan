#ifndef __TITAN_SHADER_MGR__HH
#define __TITAN_SHADER_MGR__HH

#include "TiPrerequisites.h"
#include "TiShader.h"
#include "TiShaderParams.h"
#include "TiResourceMgr.h"
#include "TiSingleton.h"

namespace Titan
{
	class _DllExport ShaderMgr : public ResourceMgr, public Singleton<ShaderMgr>
	{
	public:
		ShaderMgr();

		~ShaderMgr();

		ShaderPtr	createShader(const String& source, const String& name, const String& group, const String& language, ShaderType type);

		ShaderPtr	createShaderFromFile(const String& file, const String& name, const String& group, const String& language, ShaderType type);

		ShaderParamsPtr	createShaderParams();

		virtual ResourcePtr create(const String& name, const String& group, ShaderType type);

		


	protected:
		virtual Resource* createImpl(const String& name, ResourceHandle id, const String& group, ShaderType type) = 0; 

	public:
		static ShaderMgr*	getSingletonPtr();

		static ShaderMgr&	getSingleton();
	};
}
#endif