#include "TitanStableHeader.h"
#include "TiShaderMgr.h"
#include "TiShaderParams.h"


namespace Titan
{
	template<> ShaderMgr* Singleton<ShaderMgr>::ms_Singleton = 0;
	ShaderMgr& ShaderMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	ShaderMgr* ShaderMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}
	
	ShaderMgr::ShaderMgr()
		:ResourceMgr()
	{
		mLoadOrder = 40;
	}
	//----------------------------------------------------------------------------//
	ShaderMgr::~ShaderMgr()
	{

	}
	//----------------------------------------------------------------------------//
	ResourcePtr ShaderMgr::create(const String& name, const String& group, ShaderType type)
	{
		ResourceMap::iterator it = mResourceMap.find(name);
		if(it != mResourceMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"The shader resource with this name has existed: " + name,
				"ShaderMgr::create");
			return ResourcePtr();
		}
		ResourcePtr res = ResourcePtr(createImpl(name, getNextHandle(), group, type));
		addImpl(res);

		ResourceGroupMgr::getSingleton().addCreatedResource(group, res);
		return res;
	}
	//------------------------------------------------------------------------------//
	ShaderPtr ShaderMgr::createShader(const String& source, const String& name, const String& group, const String& language, ShaderType type)
	{

		ShaderPtr shader = create(name, group, type);
		shader->setLanguage(language);
		shader->setSource(source);
		shader->setShaderType(type);

		return shader;
	}
	//----------------------------------------------------------------------------//
	ShaderPtr ShaderMgr::createShaderFromFile(const String& file, const String& name, const String& group, const String& language, ShaderType type)
	{
		ShaderPtr shader = create(name, group, type);
		shader->setFileName(file);
		shader->setLanguage(language);
		shader->setShaderType(type);
	
		return shader;
	}
	//----------------------------------------------------------------------------//
	ShaderParamsPtr ShaderMgr::createShaderParams()
	{
		return ShaderParamsPtr(TITAN_NEW ShaderParams());
	}
}