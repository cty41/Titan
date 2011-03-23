#include "TitanStableHeader.h"
#include "TitanPixelFormat.h"
#include "Exception.h"

namespace Titan
{
	
	 /**
    * A record that describes a pixel format in detail.
    */
    struct PixelFormatDescription {
        /* Name of the format, as in the enum */
        const char *name;
        /* Number of bytes one element (colour value) takes. */
        unsigned char elemBytes;
        /* Pixel format flags, see enum PixelFormatFlags for the bit field
        * definitions
        */
        uint32 flags;
        /** Component type
         */
        PixelComponentType componentType;
        /** Component count
         */
        unsigned char componentCount;
        /* Number of bits for red(or luminance), green, blue, alpha
        */
        unsigned char rbits,gbits,bbits,abits; /*, ibits, dbits, ... */

        /* Masks and shifts as used by packers/unpackers */
        uint32 rmask, gmask, bmask, amask;
        unsigned char rshift, gshift, bshift, ashift;
    };

	PixelFormatDescription _pixelFormats[PF_COUNT] = {
		//-----------------------------------------------------------------------
		{"PF_UNKNOWN",
		/* Bytes per element */
		0,
		/* Flags */
		0,
		/* Component type and count */
		PCT_BYTE, 0,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-----------------------------------------------------------------------
		{"PF_A8",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 8,
		/* Masks and shifts */
		0, 0, 0, 0xFF, 0, 0, 0, 0
		},
		//-----------------------------------------------------------------------
		{"PF_A8R8G8B8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
		16, 8, 0, 24
		},
		//-----------------------------------------------------------------------
	};

	static inline const PixelFormatDescription &getDescriptionFor(const PixelFormat fmt)
	{
		const int ord = (int)fmt;
		assert(ord>=0 && ord<PF_COUNT);

		return _pixelFormats[ord];
	}
	size_t PixelFuncs::getNumElemBytes( PixelFormat format )
	{
		return getDescriptionFor(format).elemBytes;
	}
	//-------------------------------------------------------------//
	size_t PixelFuncs::getMemorySize(size_t width, size_t height, size_t depth, PixelFormat format)
	{
		//add compressed later
		return width * height * depth * getNumElemBytes(format);
	}
}