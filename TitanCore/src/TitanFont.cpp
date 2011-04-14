#include "TitanStableHeader.h"
#include "TitanFont.h"
#include "TitanTextureMgr.h"
#include "ConsoleDebugger.h"
#include "TitanBitOperation.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace Titan
{
	Font::Font(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:Resource(mgr, name, id, group), mTtfSize(15.0f), mTtfResolution(96), mTtfMaxBearingY(0),
		mType(FT_TRUETYPE), mAntialiasColor(false)
	{

	}
	//-------------------------------------------------------------------------------//
	Font::~Font()
	{
		unload();
	}
	//-------------------------------------------------------------------------------//
	void Font::loadImpl()
	{
		//use for material later
		bool blendByAlpha = true;
		mSource = mName;
		if(mType == FT_TRUETYPE)
		{
			createTextureFromFont();
		}
		else
		{
			mFontTexture = TextureMgr::getSingleton().load(mSource);
			blendByAlpha = mFontTexture->hasAlpha();
		}
	}
	//-------------------------------------------------------------------------------//
	void Font::unloadImpl()
	{
		if (!mFontTexture.isNull())
		{
			mFontTexture.setNull();
		}
	}
	//-------------------------------------------------------------------------------//
	void Font::createTextureFromFont()
	{
		FT_Library ftLibrary;
		// Init freetype
		if( FT_Init_FreeType( &ftLibrary ) )
			TITAN_EXCEPT( Exception::EXCEP_INTERNAL_ERROR, "Could not init FreeType library!",
			"Font::createTextureFromFont");

		FT_Face face;
		// Add a gap between letters vert and horz
		// prevents nasty artefacts when letters are too close together
		uint char_spacer = 5;

		// Load font
		if( FT_New_Memory_Face( ftLibrary, mPreparedData->getPtr(), (FT_Long)mPreparedData->size() , 0, &face ) )
			TITAN_EXCEPT( Exception::EXCEP_INTERNAL_ERROR,
			"Could not open font face!", "Font::createTextureFromFont" );


		// Convert our point size to freetype 26.6 fixed point format
		FT_F26Dot6 ftSize = (FT_F26Dot6)(mTtfSize * (1 << 6));
		if( FT_Set_Char_Size( face, ftSize, 0, mTtfResolution, mTtfResolution ) )
			TITAN_EXCEPT( Exception::EXCEP_INTERNAL_ERROR,
			"Could not set char size!", "Font::createTextureFromFont" );

		//FILE *fo_def = stdout;

		int max_height = 0, max_width = 0;

		// Backwards compatibility - if codepoints not supplied, assume 33-166
		if (mFontCodeRangeVec.empty())
		{
			mFontCodeRangeVec.push_back(FontCodeRange(33, 166));
		}

		// Calculate maximum width, height and bearing
		size_t glyphCount = 0;
		for (FontCodeRangeVec::const_iterator r = mFontCodeRangeVec.begin();
			r != mFontCodeRangeVec.end(); ++r)
		{
			const FontCodeRange& range = *r;
			for(uint32 cp = range.first; cp <= range.second; ++cp, ++glyphCount)
			{
				FT_Load_Char( face, cp, FT_LOAD_RENDER );

				if( ( 2 * ( face->glyph->bitmap.rows << 6 ) - face->glyph->metrics.horiBearingY ) > max_height )
					max_height = ( 2 * ( face->glyph->bitmap.rows << 6 ) - face->glyph->metrics.horiBearingY );
				if( face->glyph->metrics.horiBearingY > mTtfMaxBearingY )
					mTtfMaxBearingY = face->glyph->metrics.horiBearingY;

				if( (face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 ) > max_width)
					max_width = (face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 );
			}

		}

		// Now work out how big our texture needs to be
		size_t rawSize = (max_width + char_spacer) *
			((max_height >> 6) + char_spacer) * glyphCount;

		uint32 tex_side = static_cast<uint32>(Math::Sqrt((float)rawSize));
		// just in case the size might chop a glyph in half, add another glyph width/height
		tex_side += std::max(max_width, (max_height>>6));
		// Now round up to nearest power of two
		uint32 roundUpSize = firstPO2From(tex_side);

		// Would we benefit from using a non-square texture (2X width(
		size_t finalWidth, finalHeight;
		if (roundUpSize*roundUpSize*0.5 >= rawSize)
		{
			finalHeight = static_cast<size_t>(roundUpSize * 0.5);
		}
		else
		{
			finalHeight = roundUpSize;
		}
		finalWidth = roundUpSize;

		float textureAspect = (float)finalWidth / (float)finalHeight;

		const size_t pixel_bytes = 2;
		size_t data_width = finalWidth * pixel_bytes;
		size_t data_size = finalWidth * finalHeight * pixel_bytes;

		ConsoleDebugger::getSingleton().outputMessage("Font " + mName + "using texture size " +
			StringConverter::toString(finalWidth) + "x" + StringConverter::toString(finalHeight));

		uchar* imageData = TITAN_ALLOC_T(uchar, data_size, MEMCATEGORY_GENERAL);
		// Reset content (White, transparent)
		for (size_t i = 0; i < data_size; i += pixel_bytes)
		{
			imageData[i + 0] = 0xFF; // luminance
			imageData[i + 1] = 0x00; // alpha
		}

		size_t l = 0, m = 0;
		for (FontCodeRangeVec::const_iterator r = mFontCodeRangeVec.begin();
			r != mFontCodeRangeVec.end(); ++r)
		{
			const FontCodeRange& range = *r;
			for(uint32 cp = range.first; cp <= range.second; ++cp )
			{
				FT_Error ftResult;

				// Load & render glyph
				ftResult = FT_Load_Char( face, cp, FT_LOAD_RENDER );
				if (ftResult)
				{
					// problem loading this glyph, continue
					ConsoleDebugger::getSingleton().outputMessage("Info: cannot load character " +
						StringConverter::toString(cp) + " in font " + mName);
					continue;
				}

				FT_Int advance = face->glyph->advance.x >> 6;

				unsigned char* buffer = face->glyph->bitmap.buffer;

				if (!buffer)
				{
					// Yuck, FT didn't detect this but generated a null pointer!
					ConsoleDebugger::getSingleton().outputMessage("Info: Freetype returned null for character " +
						StringConverter::toString(cp) + " in font " + mName);
					continue;
				}

				int y_bearnig = ( mTtfMaxBearingY >> 6 ) - ( face->glyph->metrics.horiBearingY >> 6 );
				int x_bearing = face->glyph->metrics.horiBearingX >> 6;

				for(int j = 0; j < face->glyph->bitmap.rows; j++ )
				{
					size_t row = j + m + y_bearnig;
					uchar* pDest = &imageData[(row * data_width) + (l + x_bearing) * pixel_bytes];
					for(int k = 0; k < face->glyph->bitmap.width; k++ )
					{
						if (mAntialiasColor)
						{
							// Use the same greyscale pixel for all components RGBA
							*pDest++= *buffer;
						}
						else
						{
							// Always white whether 'on' or 'off' pixel, since alpha
							// will turn off
							*pDest++= 0xFF;
						}
						// Always use the greyscale value for alpha
						*pDest++= *buffer++; 
					}
				}

				this->setGlyphTexCoords(cp,
					(float)l / (float)finalWidth,  // u1
					(float)m / (float)finalHeight,  // v1
					(float)( l + ( face->glyph->advance.x >> 6 ) ) / (float)finalWidth, // u2
					( m + ( max_height >> 6 ) ) / (float)finalHeight, // v2
					textureAspect
					);

				// Advance a column
				l += (advance + char_spacer);

				// If at end of row
				if( finalWidth - 1 < l + ( advance ) )
				{
					m += ( max_height >> 6 ) + char_spacer;
					l = 0;
				}
			}
		}


		mFontTexture = TextureMgr::getSingleton().createManually("Font/"+ mSource, ResourceGroupMgr::GENERAL_RESOURCE_GROUP, TT_2D, finalWidth, finalHeight,1,TU_DYNAMIC, PF_BYTE_LA);
		
		PixelBox	lockedRect;
		mFontTexture->lockRect(0, &lockedRect, NULL, HardwareBuffer::HBL_DISCARD);
		uchar* TexData = (uchar*)lockedRect.data;

		for(uint i = 0; i < finalHeight; ++i)
		{
			for(uint j = 0; j < finalWidth; ++j)
			{
				size_t index = i * lockedRect.rowPitch * pixel_bytes + j * pixel_bytes;
				TexData[index] = imageData[index];
				TexData[index + 1] = imageData[index + 1];
			}
		}

		mFontTexture->unlockRect(0);

		FT_Done_FreeType(ftLibrary);
	}
}