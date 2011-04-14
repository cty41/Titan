
#include "TitanStableHeader.h"
#include "TiPass.h"
#include "TiTextureUnit.h"

namespace Titan
{
	Pass::Pass(Material* parent, size_t index)
		:mParent(parent), mIndex(index),mHashCode(0),mUseShader(false),
		mCullMode(CULL_COUNTERCLOCKWISE), mPolygonMode(PM_SOLID), 
		mShadeMode(SO_GOURAUD), mSrcBlendFactor(SBF_SOURCE_ALPHA), mDstBlendFactor(SBF_ONE_MINUS_SOURCE_ALPHA),
		mSrcBlendFactorAlpha(SBF_ONE), mDstBlendFactorAlpha(SBF_ZERO),
		mBlendOperation(SBO_ADD), mAlphaBlendOperation(SBO_ADD), mDepthCheck(true),
		mDepthWrite(true),mDepthFunc(CMPF_LESS_EQUAL), mSortTransparent(true),
		mIsLightEnable(false)

	{
	}
	//-------------------------------------------------------------------------------//
	Pass::~Pass()
	{
		removeAllTextureUnits();
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