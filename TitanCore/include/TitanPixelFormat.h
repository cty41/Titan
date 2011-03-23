#ifndef __TITAN_PIXEL_FORMAT__HH
#define __TITAN_PIXEL_FORMAT__HH

#include "TitanPrerequisites.h"
#include "TitanCommon.h"

namespace Titan
{
	enum PixelFormat
	{
		PF_UNKNOWN	= 0,
		PF_A8		= 1,
		PF_A8R8G8B8	= 2,
		PF_COUNT,
	};

	    /**
     * Flags defining some on/off properties of pixel formats
     */
    enum PixelFormatFlags {
        // This format has an alpha channel
        PFF_HASALPHA        = 0x00000001,      
        // This format is compressed. This invalidates the values in elemBytes,
        // elemBits and the bit counts as these might not be fixed in a compressed format.
        PFF_COMPRESSED    = 0x00000002,
        // This is a floating point format
        PFF_FLOAT           = 0x00000004,         
        // This is a depth format (for depth textures)
        PFF_DEPTH           = 0x00000008,
        // Format is in native endian. Generally true for the 16, 24 and 32 bits
        // formats which can be represented as machine integers.
        PFF_NATIVEENDIAN    = 0x00000010,
        // This is an intensity format instead of a RGB one. The luminance
        // replaces R,G and B. (but not A)
        PFF_LUMINANCE       = 0x00000020
    };

	/** Pixel component format */
	enum PixelComponentType
	{
		PCT_BYTE = 0,    /// Byte per component (8 bit fixed 0.0..1.0)
		PCT_SHORT = 1,   /// Short per component (16 bit fixed 0.0..1.0))
		PCT_FLOAT16 = 2, /// 16 bit float per component
		PCT_FLOAT32 = 3, /// 32 bit float per component
		PCT_COUNT = 4    /// Number of pixel types
	};


	class _DllExport PixelFuncs
	{
	public:
		static size_t getMemorySize(size_t width, size_t height, size_t depth, PixelFormat format);

		static size_t PixelFuncs::getNumElemBytes( PixelFormat format );
	};

	class _DllExport PixelBox: public Box
	{
	public:
		PixelBox() {}
		~PixelBox() {}

		PixelBox(const Box &extents, PixelFormat pixelFormat, void *pixelData=0):
		Box(extents), data(pixelData), format(pixelFormat)
		{
			setConsecutive();
		}

		PixelBox(size_t width, size_t height, size_t depth, PixelFormat pixelFormat, void *pixelData=0):
		Box(0, 0, 0, width, height, depth),
			data(pixelData), format(pixelFormat)
		{
			setConsecutive();
		}

		/// The data pointer 
		void *data;
		/// The pixel format 
		PixelFormat format;
		/** Number of elements between the leftmost pixel of one row and the left
         	pixel of the next. This value must always be equal to getWidth() (consecutive) 
			for compressed formats.
			PS: be aware in D3DLOCKED_RECT the Pitch is the Number of bytes in one row of the surface. Here the rowPitch == D3D Pitch / bpp
        */
        size_t rowPitch;
        /** Number of elements between the top left pixel of one (depth) slice and 
         	the top left pixel of the next. This can be a negative value. Must be a multiple of
         	rowPitch. This value must always be equal to getWidth()*getHeight() (consecutive) 
			for compressed formats.
        */
        size_t slicePitch;
        /** Set the rowPitch and slicePitch so that the buffer is laid out consecutive 
         	in memory.
        */   
		void setConsecutive()
		{
			rowPitch = getWidth();
			slicePitch = getWidth()*getHeight();
		}
         /**	Get the number of elements between one past the rightmost pixel of 
         	one row and the leftmost pixel of the next row. (IE this is zero if rows
         	are consecutive).
        */
        size_t getRowSkip() const { return rowPitch - getWidth(); }
        /** Get the number of elements between one past the right bottom pixel of
         	one slice and the left top pixel of the next slice. (IE this is zero if slices
         	are consecutive).
        */
        size_t getSliceSkip() const { return slicePitch - (getHeight() * rowPitch); }

        /** Return whether this buffer is laid out consecutive in memory (ie the pitches
         	are equal to the dimensions)
        */        
        bool isConsecutive() const 
		{ 
			return rowPitch == getWidth() && slicePitch == getWidth()*getHeight(); 
		}
        /** Return the size (in bytes) this image would take if it was
        	laid out consecutive in memory
      	*/
      	size_t getConsecutiveSize() const
		{
			return PixelFuncs::getMemorySize(getWidth(), getHeight(), getDepth(), format);
		}
	};
}

#endif