#include "TitanStableHeader.h"
#include "TiTextureUnit.h"
#include "TiStringConverter.h"
#include "TiTextureMgr.h"
#include "TiPass.h"

namespace Titan
{
	TextureUnit::MinFilterCmd	TextureUnit::msMinFilterCmd;
	TextureUnit::MagFilterCmd	TextureUnit::msMagFilterCmd;
	TextureUnit::MipFilterCmd	TextureUnit::msMipFilterCmd;
	TextureUnit::TexAddressModeCmd	TextureUnit::msTexAddressModeCmd;
	TextureUnit::TexCoordSetCmd	TextureUnit::msTexCoordSetCmd;


	TextureUnit::TextureUnit(Pass* parent)
		:mParent(parent), mMagFilter(FO_LINEAR), mMinFilter(FO_LINEAR), mMipFilter(FO_NONE),
		mTexCoordSet(0), mRecalcTexMatrix(false), mTexMatrix(Matrix4::IDENTITY)
	{
		setTexAddressMode(TAM_WARP);

		if(createClassParamsDict("texture_unit"))
			setupParamsCmd();
	}
	//----------------------------------------------------------------------------//
	TextureUnit::~TextureUnit()
	{

	}
	//----------------------------------------------------------------------------//
	void TextureUnit::_load()
	{
		if(mTexture.isNull())
		{
			mTexture = TextureMgr::getSingleton().load(mTexName, mParent->getGroup(), mType);
		}
	}
	//----------------------------------------------------------------------------//
	void TextureUnit::_unload()
	{
		if(!mTexture.isNull())
			mTexture->unload();
	}
	//----------------------------------------------------------------------------//
	void TextureUnit::setTexture(const String& name, TexType type)
	{
		mTexName = name;
		mType = type;
	}
	const TexturePtr& TextureUnit::getTexture()
	{
		if (mTexture.isNull())
		{
			mTexture = TextureMgr::getSingleton().load(mTexName, mParent->getGroup(), mType);
			
		}
		return mTexture;
	}
	//------------------------------------------------------------------------------//
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
	//------------------------------------------------------------------------------//
	void TextureUnit::setTextureTransform(const Matrix4& transform)
	{
		mTexMatrix = transform;
		mRecalcTexMatrix = false;
	}
	const Matrix4& TextureUnit::getTextureTransform()
	{
		if(mRecalcTexMatrix)
			recalcTextureMatrix();
		
		return mTexMatrix;
	}
	//------------------------------------------------------------------------------//
	void TextureUnit::recalcTextureMatrix()
	{
		//add later
		mRecalcTexMatrix = false;
	}
	//------------------------------------------------------------------------------//
	void TextureUnit::setupParamsCmd()
	{
		mClassParamsDict->addParamsCommand("min_filter", &msMinFilterCmd);
		mClassParamsDict->addParamsCommand("mag_filter", &msMagFilterCmd);
		mClassParamsDict->addParamsCommand("mip_filter", &msMipFilterCmd);
		mClassParamsDict->addParamsCommand("tex_address_mode", &msTexAddressModeCmd);
		mClassParamsDict->addParamsCommand("texcoord_set", &msTexCoordSetCmd);
	}

	void TextureUnit::MinFilterCmd::setter(void* target, const String& val)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		int fo = mapToFilterOptions(val);
		if (fo == FO_INVALID)
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"Min Filter does not support this value: " + val,
				"TextureUnit::MinFilterCmd::setter");
		}
		else
			tu->setMinFilter(FilterOptions(fo));
	}
	//------------------------------------------------------------------------------//
	String TextureUnit::MinFilterCmd::getter(void* target)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		return FilterOptionsToString(tu->getMinFilter());
	}
	//------------------------------------------------------------------------------//
	void TextureUnit::MagFilterCmd::setter(void* target, const String& val)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		int fo = mapToFilterOptions(val);
		if (fo == FO_INVALID)
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"Mag Filter does not support this value: " + val,
				"TextureUnit::MagFilterCmd::setter");
		}
		else
			tu->setMagFilter(FilterOptions(fo));
	}
	//------------------------------------------------------------------------------//
	String TextureUnit::MagFilterCmd::getter(void* target)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		return FilterOptionsToString(tu->getMagFilter());
	}
	//------------------------------------------------------------------------------//
	void TextureUnit::MipFilterCmd::setter(void* target, const String& val)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		int fo = mapToFilterOptions(val);
		if (fo == FO_INVALID)
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"Mip Filter does not support this value: " + val,
				"TextureUnit::MipFilterCmd::setter");
		}
		else
			tu->setMipFilter(FilterOptions(fo));
	}
	//------------------------------------------------------------------------------//
	String TextureUnit::MipFilterCmd::getter(void* target)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		return FilterOptionsToString(tu->getMipFilter());
	}
	//------------------------------------------------------------------------------//
	void TextureUnit::TexAddressModeCmd::setter(void* target, const String& val)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		if (val == "wrap")
		{
			tu->setTexAddressMode(TAM_WARP);
		}
		else if(val == "mirror")
		{
			tu->setTexAddressMode(TAM_MIRROR);
		}
		else if(val == "clamp")
		{
			tu->setTexAddressMode(TAM_CLAMP);
		}
		else if(val == "border")
		{
			tu->setTexAddressMode(TAM_BORDER);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"texture address mode does not support this value: " + val,
				"TextureUnit::TexAddressModeCmd::setter");
		}
	}
	//------------------------------------------------------------------------------//
	String TextureUnit::TexAddressModeCmd::getter(void* target)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		switch(tu->getTexAddressModeSets().UTexAddrMode)
		{
		case TAM_WARP:
			return "wrap";
		case TAM_MIRROR:
			return "mirror";
		case TAM_CLAMP:
			return "clamp";
		case TAM_BORDER:
			return "border";
		}
		return "";
	}
	//------------------------------------------------------------------------------//
	void TextureUnit::TexCoordSetCmd::setter(void* target, const String& val)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		int texId = StringConverter::parseInt(val);
		if(texId>= 0 && texId < 8)
		{
			tu->setTexCoordSet(texId);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"We only support 8 sets of texcoords",
				"TextureUnit::TexCoordSetCmd::setter");
		}

	}
	//------------------------------------------------------------------------------//
	String TextureUnit::TexCoordSetCmd::getter(void* target)
	{
		TextureUnit* tu = static_cast<TextureUnit*>(target);
		return StringConverter::toString(tu->getTexCoordSet());
	}
	//------------------------------------------------------------------------------//

	FilterOptions TextureUnit::mapToFilterOptions(const String& val)
	{
		if(val == "none")
			return FO_NONE;
		else if(val == "point")
			return FO_POINT;
		else if(val == "linear")
			return FO_LINEAR;
		else
			return FO_INVALID;
	}
	//------------------------------------------------------------------------------//
	String TextureUnit::FilterOptionsToString(FilterOptions fo)
	{
		switch(fo)
		{
		case FO_NONE:
			return "none";
		case FO_POINT:
			return "point";
		case FO_LINEAR:
			return "linear";
		}
		return "";
	}
}