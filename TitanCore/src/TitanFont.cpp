#include "TitanStableHeader.h"
#include "TitanFont.h"
#include "TitanTextureMgr.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace Titan
{
	Font::Font(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:Resource(mgr, name, id, group), mTtfSize(0.0f), mTtfResolution(0), mTtfMaxBearingY(0)
	{

	}
	//-------------------------------------------------------------//
	Font::~Font()
	{
		unload();
	}
	//-------------------------------------------------------------//
	void Font::loadImpl()
	{
		createTextureFromFont();

	}
	//-------------------------------------------------------------//
	void Font::unloadImpl()
	{
		if (mFontTexture.isNull())
		{
		}
	}
	//-------------------------------------------------------------//
	void Font::createTextureFromFont()
	{

	}
}