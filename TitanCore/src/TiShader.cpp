#include "TitanStableHeader.h"
#include "TiShader.h"
#include "TiShaderMgr.h"
#include "TiConsoleDebugger.h"
#include "TiDataStream.h"
#include "TiResourceGroupMgr.h"

namespace Titan
{
	Shader::ShaderFileCmd	Shader::msShaderFileCmd;
	Shader::ShaderSrcCmd	Shader::msShaderSrcCmd;
	Shader::ShaderLanguageCmd Shader::msShaderLanguageCmd;

	Shader::Shader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual)
		:Resource(mgr, name, id, group, isManual), mHasCompiledError(false), mConstantDefsBuilt(false),
		mLoadFromFile(false)
	{

	}
	//----------------------------------------------------------------------------//
	inline void Shader::setSource(const String& source)
	{
		mSource = source;
		mFile.clear();
		mLoadFromFile = false;
		mHasCompiledError = false;
	}
	//------------------------------------------------------------------------------//
	inline void Shader::setFileName(const String& fileName)
	{
		mFile = fileName;
		mSource.clear();
		mLoadFromFile = true;
		mHasCompiledError = false;
	}
	//------------------------------------------------------------------------------//
	void Shader::loadImpl()
	{
		try
		{
			if (mLoadFromFile)
			{
				DataStreamPtr stream = ResourceGroupMgr::getSingleton().openResource(mFile, mGroup);

				mSource = stream->getAsString();
			}
			loadFromSrc();
		}
		catch(const Exception& e)
		{
			DEBUG_OUTPUT<<"shader named" + mName + "got error when loading"<<e.getFullDescription();
			mHasCompiledError = true;
		}
	}
	//----------------------------------------------------------------------------//
	ShaderParamsPtr Shader::createShaderParams()
	{
		ShaderParamsPtr params = ShaderMgr::getSingleton().createShaderParams();
		load();
		setParamsNames(params);

		// Copy in default parameters if present
		if (!mShaderParams.isNull())
		{
			params->copyConstants(*(mShaderParams.get()));
		}
		
		return params;
	}
	//----------------------------------------------------------------------------//
	ShaderParamsPtr Shader::getShaderParams()
	{
		if (mShaderParams.isNull())
		{
			mShaderParams = createShaderParams();
		}
		return mShaderParams;
	}
	//----------------------------------------------------------------------------//
	const ShaderNamedConstants& Shader::getConstantDefs()
	{
		if (!mConstantDefsBuilt)
		{
			buildConstantDefs();
			mConstantDefsBuilt = true;
		}

		return *mConstantDefs.get();
	}
	//------------------------------------------------------------------------------//
	void Shader::setParamsNames(ShaderParamsPtr params)
	{
		getConstantDefs();
		params->_setNamedConstants(mConstantDefs);
		params->_setRegisterIndexes(mFloatRegisterToPhysical, mIntRegisterToPhysical);
	}
	//------------------------------------------------------------------------------//
	void Shader::createParamMappingBuffer(bool recreateIfExists)
	{
		createNamedParamMappingBuffer(recreateIfExists);
		createRegisterParamMappingBuffer(recreateIfExists);
	}
	//------------------------------------------------------------------------------//
	void Shader::createRegisterParamMappingBuffer(bool recreateIfExists)
	{
		if (recreateIfExists || mFloatRegisterToPhysical.isNull())
			mFloatRegisterToPhysical = ShaderRegisterBufferPtr(TITAN_NEW ShaderRegisterBuffer());
		if (recreateIfExists || mIntRegisterToPhysical.isNull())
			mIntRegisterToPhysical = ShaderRegisterBufferPtr(TITAN_NEW ShaderRegisterBuffer());
	}
	//------------------------------------------------------------------------------//
	void Shader::createNamedParamMappingBuffer(bool recreateIfExists)
	{
		if (recreateIfExists || mConstantDefs.isNull())
			mConstantDefs = ShaderNamedConstantsPtr(TITAN_NEW ShaderNamedConstants());
	}
	//------------------------------------------------------------------------------//
	void Shader::setupParamsCmd()
	{
		mClassParamsDict->addParamsCommand("src", &msShaderSrcCmd);
		mClassParamsDict->addParamsCommand("file", &msShaderFileCmd);
		mClassParamsDict->addParamsCommand("language", &msShaderLanguageCmd);
	}

	void Shader::ShaderFileCmd::setter(void* target, const String& val)
	{
		Shader* shader = static_cast<Shader*>(target);
		shader->setFileName(val);
	}
	//------------------------------------------------------------------------------//
	String Shader::ShaderFileCmd::getter(void* target)
	{
		Shader* shader = static_cast<Shader*>(target);
		return shader->getFileName();
	}
	//------------------------------------------------------------------------------//
	void Shader::ShaderSrcCmd::setter(void* target, const String& val)
	{
		Shader* shader = static_cast<Shader*>(target);
		shader->setSource(val);
	}
	//------------------------------------------------------------------------------//
	String Shader::ShaderSrcCmd::getter(void* target)
	{
		Shader* shader = static_cast<Shader*>(target);
		return shader->getSource();
	}
	//------------------------------------------------------------------------------//
	void Shader::ShaderLanguageCmd::setter(void* target, const String& val)
	{
		Shader* shader = static_cast<Shader*>(target);
		shader->setLanguage(val);
	}
	//------------------------------------------------------------------------------//
	String Shader::ShaderLanguageCmd::getter(void* target)
	{
		Shader* shader = static_cast<Shader*>(target);
		return shader->getLanguage();
	}
}