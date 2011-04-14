#include "TitanStableHeader.h"
#include "TiTextureUnit.h"

namespace Titan
{
	TextureUnit::TextureUnit()
		:mMaxFilter(FO_POINT), mMinFilter(FO_POINT), mMipFilter(FO_NONE),
		mTexCoordSet(0)
	{
		setTexAddressMode(TAM_WARP);
	}
	//----------------------------------------------------------------------------//
	TextureUnit::~TextureUnit()
	{

	}
	//----------------------------------------------------------------------------//
	void TextureUnit::_load()
	{

	}
	//----------------------------------------------------------------------------//
	void TextureUnit::_unload()
	{

	}
	//----------------------------------------------------------------------------//
	void TextureUnit::setTexAddressMode(TexAddressMode tam)
	{
		mTexAddressModeSets.WTexAddrMode = mTexAddressModeSets.VTexAddrMode = mTexAddressModeSets.UTexAddrMode = tam;
	}
	//----------------------------------------------------------------------------//
	void TextureUnit::setTexAddressMode(TexAddressMode uMode, TexAddressMode vMode, TexAddressMode wMode)
	{
		mTexAddressModeSets.UTexAddrMode = uMode;
		mTexAddressModeSets.VTexAddrMode = vMode;
		mTexAddressModeSets.WTexAddrMode = wMode;
	}

}