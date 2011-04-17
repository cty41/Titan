#include "TitanStableHeader.h"
#include "TiPass.h"
#include "TiTextureUnit.h"
#include "TiStringFuncs.h"

namespace Titan
{
	//we need to change here mSrcBlendFactor and mDstlendFactor
	Pass::Pass(Material* parent, size_t index)
		:mParent(parent), mIndex(index),mHashCode(0),
		mCullMode(CULL_COUNTERCLOCKWISE), mPolygonMode(PM_SOLID), 
		mShadeMode(SO_GOURAUD), mSrcBlendFactor(SBF_ONE), 
		mDstBlendFactor(SBF_ZERO),
		mSrcBlendFactorAlpha(SBF_ONE), mDstBlendFactorAlpha(SBF_ZERO),
		mBlendOperation(SBO_ADD), mAlphaBlendOperation(SBO_ADD), mDepthCheck(true),
		mDepthWrite(true),mDepthFunc(CMPF_LESS_EQUAL), mSortTransparent(true),
		mIsLightEnable(false), mVertexShaderUnit(NULL), mPixelShaderUnit(NULL)

	{
	}
	//-------------------------------------------------------------------------------//
	Pass::~Pass()
	{
		removeAllTextureUnits();

		if(mVertexShaderUnit)
			TITAN_DELETE mVertexShaderUnit;

		if(mPixelShaderUnit)
			TITAN_DELETE mPixelShaderUnit;
	}
	//-------------------------------------------------------------------------------//
	TextureUnit* Pass::createTextureUnit()
	{
		TextureUnit* tu = TITAN_NEW TextureUnit();
		mTextureUnitVec.push_back(tu);

		return tu;
	}
	//-------------------------------------------------------------------------------//
	void Pass::removeAllTextureUnits()
	{
		TextureUnitVec::iterator it = mTextureUnitVec.begin(), itend = mTextureUnitVec.end();
		for(;it != itend;++it)
		{
			TITAN_DELETE (*it);
		}
		mTextureUnitVec.clear();
	}
	//-------------------------------------------------------------------------------//
	void Pass::updateAutoParams(AutoParamsSetter* setter) const
	{
		if(hasVertexShader())
		{
			mVertexShaderUnit->getShaderParams()->updateAutoParams(setter);
		}

		if(hasPixelShader())
		{
			mPixelShaderUnit->getShaderParams()->updateAutoParams(setter);
		}
	}
	//------------------------------------------------------------------------------//
	const String&  Pass::getVertexShaderName() const
	{
		if (!mVertexShaderUnit)
		{
			return StringUtil::BLANK;
		}
		else
			return mVertexShaderUnit->getShader()->getName();
	}
	//------------------------------------------------------------------------------//
	const String&  Pass::getPixelShaderName() const
	{
		if (!mPixelShaderUnit)
		{
			return StringUtil::BLANK;
		}
		else
			return mPixelShaderUnit->getShader()->getName();
	}
	//------------------------------------------------------------------------------//
	void Pass::setVertexShader(const String& name)
	{
		if (getVertexShaderName() != name)
		{
			if(name.empty())
			{
				if(mVertexShaderUnit)
					TITAN_DELETE mVertexShaderUnit;
				mVertexShaderUnit = NULL;
			}
			else
			{
				if (!mVertexShaderUnit)
				{
					mVertexShaderUnit = TITAN_NEW ShaderUnit(this, ST_VERTEX_SHADER);
				}
				mVertexShaderUnit->setShader(name);
			}
		}
		else
		{
			if(!mVertexShaderUnit)
			{
				mVertexShaderUnit = TITAN_NEW ShaderUnit(this, ST_VERTEX_SHADER);
			}
			mVertexShaderUnit->setShader(name);
		}
	}
	//------------------------------------------------------------------------------//
	void Pass::setPixelShader(const String& name)
	{
		if (getPixelShaderName() != name)
		{
			if(name.empty())
			{
				if(mPixelShaderUnit)
					TITAN_DELETE mPixelShaderUnit;
				mPixelShaderUnit = NULL;
			}
			else
			{
				if (!mPixelShaderUnit)
				{
					mPixelShaderUnit = TITAN_NEW ShaderUnit(this, ST_PIXEL_SHADER);
				}
				mPixelShaderUnit->setShader(name);
			}
		}
		else
		{
			if(!mPixelShaderUnit)
			{
				mPixelShaderUnit = TITAN_NEW ShaderUnit(this, ST_PIXEL_SHADER);
			}
			mPixelShaderUnit->setShader(name);
		}
	}
	//------------------------------------------------------------------------------//
	void Pass::_load()
	{
		TextureUnitVec::iterator it = mTextureUnitVec.begin(), itend = mTextureUnitVec.end();
		for(;it != itend;++it)
			(*it)->_load();

		//to do
	}
	//-------------------------------------------------------------------------------//
	void Pass::_unload()
	{
		TextureUnitVec::iterator it = mTextureUnitVec.begin(), itend = mTextureUnitVec.end();
		for(;it != itend;++it)
			(*it)->_unload();
	}
	
}