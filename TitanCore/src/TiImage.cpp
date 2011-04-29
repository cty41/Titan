#include "TitanStableHeader.h"
#include "TiImage.h"

namespace Titan
{
	Image::Image()
		:mWidth(0), mHeight(0), mSize(0),
		mData(NULL), mFormat(PF_UNKNOWN), mBytesPerPixel(0)
	{
	}
	//------------------------------------------------------------------------------//
	Image::Image(uint width, uint height, PixelFormat format, size_t depth)
		:mWidth(width), mHeight(height), mFormat(format), mDepth(depth), mBytesPerPixel(0)
	{
		mSize = PixelFuncs::getMemorySize(mWidth, mHeight, mDepth, mFormat);
		mData = TITAN_NEW_ARRAY_T(uchar, mSize, MEMCATEGORY_GENERAL);
	}
	//------------------------------------------------------------------------------//
	Image::~Image()
	{
		freeData();
	}
	//------------------------------------------------------------------------------//
	void Image::freeData()
	{
		if (mData)
		{
			TITAN_DELETE_T(mData, uchar, MEMCATEGORY_GENERAL);
			mData = NULL;
		}
	}
	//------------------------------------------------------------------------------//
	void Image::loadRawData(uchar* data, uint width, uint height, PixelFormat format)
	{
		loadRawData(data, width, height, format, 1);
	}
	//------------------------------------------------------------------------------//
	void Image::loadRawData(uchar* data, uint width, uint height, PixelFormat format, size_t depth)
	{
		freeData();

		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mFormat = format;
		mData = data;
		mBytesPerPixel = PixelFuncs::getNumElemBytes(mFormat);
		mSize = PixelFuncs::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}
	//------------------------------------------------------------------------------//
	void Image::randomChannelNoise(uint8 channel, uint8 minRange, uint8 maxRange)
	{
		int channelShift = channel*8;
		int channelMask = 0xff<<channelShift;
		int pitch = mWidth * mBytesPerPixel;
		assert(channel < 4);

		uchar* pOut = mData;
		for(uint16 y = 0; y < mHeight; ++y)
		{
			for(uint16 x = 0; x < mWidth; ++x)
			{
				uint32 color;
				uint8 noise = Math::RangeRandom(minRange, maxRange);
				
				getColor(x, y, color);

				color &= ~channelMask;
				color |= noise << channelShift;

				setColor(x, y, color);
			}
			pOut += pitch;
		}
	}
	//------------------------------------------------------------------------------//
	void Image::getColor(uint16 x, uint16 y, uint32& color) const
	{
		assert(mData && "no image");
		assert(x < mWidth && "invalid x index");
		assert(y < mHeight && "invalid y index");

		uint8* startOfColor = &mData[(y * mBytesPerPixel * mWidth) + (x * mBytesPerPixel)];
		uint8* output = (uint8*)&color;

		color = 0;

		uint16 copyCount = minimum((uint16)sizeof(color), (uint16)mBytesPerPixel);
		for (int i=0; i<copyCount; ++i)
		{
			*output++ = *startOfColor++;
		}
	}
	//------------------------------------------------------------------------------//
	void Image::setColor(uint16 x, uint16 y, uint32 color)
	{
		assert(mData && "no image");
		assert(x < mWidth && "invalid x index");
		assert(y < mHeight && "invalid y index");

		uint8* startOfColor = &mData[(y * mBytesPerPixel * mWidth) + (x * mBytesPerPixel)];
		uint8* output = (uint8*)&color;

		uint16 copyCount = minimum((uint16)sizeof(color), (uint16)mBytesPerPixel);
		for (int i=0; i<copyCount; ++i)
		{
			*startOfColor++ = *output++;
		}
	}
}