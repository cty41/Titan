#include "TitanStableHeader.h"
#include "TiPixelFormat.h"
#include "TiException.h"

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
		//-------------------------------------------------------------------------------//--
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
		//-------------------------------------------------------------------------------//--
		{"PF_L8",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_LUMINANCE | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		8, 0, 0, 0,
		/* Masks and shifts */
		0xFF, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_L16",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_LUMINANCE | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_SHORT, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0xFFFF, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
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
		//-------------------------------------------------------------------------------//--
		{"PF_A4L4",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_HASALPHA | PFF_LUMINANCE | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		4, 0, 0, 4,
		/* Masks and shifts */
		0x0F, 0, 0, 0xF0, 0, 0, 0, 4
		},
		//-------------------------------------------------------------------------------//--
		{"PF_BYTE_LA",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_HASALPHA | PFF_LUMINANCE,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		8, 0, 0, 8,
		/* Masks and shifts */
		0,0,0,0,0,0,0,0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_R5G6B5",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		5, 6, 5, 0,
		/* Masks and shifts */
		0xF800, 0x07E0, 0x001F, 0,
		11, 5, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_B5G6R5",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		5, 6, 5, 0,
		/* Masks and shifts */
		0x001F, 0x07E0, 0xF800, 0,
		0, 5, 11, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_A4R4G4B4",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		4, 4, 4, 4,
		/* Masks and shifts */
		0x0F00, 0x00F0, 0x000F, 0xF000,
		8, 4, 0, 12
		},
		//-------------------------------------------------------------------------------//--
		{"PF_A1R5G5B5",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		5, 5, 5, 1,
		/* Masks and shifts */
		0x7C00, 0x03E0, 0x001F, 0x8000,
		10, 5, 0, 15,
		},
		//-------------------------------------------------------------------------------//--
		{"PF_R8G8B8",
		/* Bytes per element */
		3,  // 24 bit integer -- special
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 0,
		/* Masks and shifts */
		0xFF0000, 0x00FF00, 0x0000FF, 0,
		16, 8, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_B8G8R8",
		/* Bytes per element */
		3,  // 24 bit integer -- special
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 0,
		/* Masks and shifts */
		0x0000FF, 0x00FF00, 0xFF0000, 0,
		0, 8, 16, 0
		},
		//-------------------------------------------------------------------------------//--
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
		//-------------------------------------------------------------------------------//--
		{"PF_A8B8G8R8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24,
		},
		//-------------------------------------------------------------------------------//--
		{"PF_B8G8R8A8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF,
		8, 16, 24, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_A2R10G10B10",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		10, 10, 10, 2,
		/* Masks and shifts */
		0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000,
		20, 10, 0, 30
		},
		//-------------------------------------------------------------------------------//--
		{"PF_A2B10G10R10",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		10, 10, 10, 2,
		/* Masks and shifts */
		0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000,
		0, 10, 20, 30
		},
		//-------------------------------------------------------------------------------//--
		{"PF_DXT1",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 3, // No alpha
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_DXT2",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_DXT3",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_DXT4",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_DXT5",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT16_RGB",
		/* Bytes per element */
		6,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT16, 3,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT16_RGBA",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_FLOAT | PFF_HASALPHA,
		/* Component type and count */
		PCT_FLOAT16, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT32_RGB",
		/* Bytes per element */
		12,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 3,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT32_RGBA",
		/* Bytes per element */
		16,
		/* Flags */
		PFF_FLOAT | PFF_HASALPHA,
		/* Component type and count */
		PCT_FLOAT32, 4,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 32,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_X8R8G8B8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 0,
		/* Masks and shifts */
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
		16, 8, 0, 24
		},
		//-------------------------------------------------------------------------------//--
		{"PF_X8B8G8R8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 0,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24
		},
		//-------------------------------------------------------------------------------//--
		{"PF_R8G8B8A8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF,
		24, 16, 8, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_DEPTH",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_DEPTH,
		/* Component type and count */
		PCT_FLOAT32, 1, // ?
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_SHORT_RGBA",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_HASALPHA,
		/* Component type and count */
		PCT_SHORT, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_R3G3B2",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		3, 3, 2, 0,
		/* Masks and shifts */
		0xE0, 0x1C, 0x03, 0,
		5, 2, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT16_R",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT16, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT32_R",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 1,
		/* rbits, gbits, bbits, abits */
		32, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_SHORT_GR",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_NATIVEENDIAN,
		/* Component type and count */
		PCT_SHORT, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0, 0, 
		0, 16, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT16_GR",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT16, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_FLOAT32_GR",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 2,
		/* rbits, gbits, bbits, abits */
		32, 32, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_SHORT_RGB",
		/* Bytes per element */
		6,
		/* Flags */
		0,
		/* Component type and count */
		PCT_SHORT, 3,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_PVRTC_RGB2",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_PVRTC_RGBA2",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_PVRTC_RGB4",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},
		//-------------------------------------------------------------------------------//--
		{"PF_PVRTC_RGBA4",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
		},

		//-------------------------------------------------------------------------------//--
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
	//-------------------------------------------------------------------------------//
	size_t PixelFuncs::getMemorySize(size_t width, size_t height, size_t depth, PixelFormat format)
	{
		//add compressed later
		return width * height * depth * getNumElemBytes(format);
	}
	//-------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------//
	unsigned int PixelFuncs::getFlags( PixelFormat format )
	{
		return getDescriptionFor(format).flags;
	}
	//-------------------------------------------------------------------------------//
	bool PixelFuncs::hasAlpha(PixelFormat format)
	{
		return (PixelFuncs::getFlags(format) & PFF_HASALPHA) > 0;
	}
	bool PixelFuncs::isFloatingPoint(PixelFormat format)
	{
		return (PixelFuncs::getFlags(format) & PFF_FLOAT) > 0;
	}
	//-------------------------------------------------------------------------------//--
	bool PixelFuncs::isCompressed(PixelFormat format)
	{
		return (PixelFuncs::getFlags(format) & PFF_COMPRESSED) > 0;
	}
	//-------------------------------------------------------------------------------//--
	bool PixelFuncs::isDepth(PixelFormat format)
	{
		return (PixelFuncs::getFlags(format) & PFF_DEPTH) > 0;
	}
	//-------------------------------------------------------------------------------//--
	bool PixelFuncs::isNativeEndian(PixelFormat format)
	{
		return (PixelFuncs::getFlags(format) & PFF_NATIVEENDIAN) > 0;
	}
	//-------------------------------------------------------------------------------//--
	bool PixelFuncs::isLuminance(PixelFormat format)
	{
		return (PixelFuncs::getFlags(format) & PFF_LUMINANCE) > 0;
	}
}