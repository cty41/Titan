#include "TitanStableHeader.h"
#include "TiShaderUnit.h"
#include "TiShaderMgr.h"

namespace Titan
{
	ShaderUnit::ShaderUnit(Pass* parent, ShaderType type)
		:mParent (parent), mType(type)
	{
	}
	//------------------------------------------------------------------------------//
	void ShaderUnit::setShader(const String& name)
	{
		mShader =ShaderMgr::getSingletonPtr()->getByName(name);
		assert(mType == mShader->getShaderType() &&" shader type must be same");
		mShaderParams = mShader->createShaderParams();
	}
	//------------------------------------------------------------------------------//
	void ShaderUnit::_load()
	{
		if (!mShader->isLoaded())
		{
			mShader->load();
		}
	}
	//------------------------------------------------------------------------------//
	void ShaderUnit::_unload()
	{
		if (mShader->isLoaded())
		{
			mShader->unload();
		}
	}

}