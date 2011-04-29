#ifndef __TITAN_IMAGE__HH
#define __TITAN_IMAGE__HH

#include "TiPrerequisites.h"
#include "TiPixelFormat.h"
#include "TiCommon.h"

namespace Titan
{
	class _DllExport Image: public GeneralAlloc
	{
	public:
		Image();

		Image(uint width, uint height, PixelFormat format, size_t depth = 1);

		~Image();

		void	load(const String& fileName, const String& group);

		void	loadRawData(uchar* data, uint width, uint height, PixelFormat format);

		void	loadRawData(uchar* data, uint width, uint height, PixelFormat format, size_t depth);

		void	setWidth(uint width){ mWidth = width; }

		uint	getWidth() const { return mWidth;}

		void	setHeight(uint height){ mHeight = height;}

		uint	getHeight() const { return mHeight; }

		void	setDepth(uint depth){ mDepth = depth; }

		uint	getDepth() const { return mDepth; }

		uchar*	getData(){ return mData; }

		uchar*	getData() const { return mData; }

		PixelFormat	getFormat() const { return mFormat; }

		void	setFormat(PixelFormat format){ mFormat = format; }

		uint	getSize() const { return mSize; }

		uint	getBytesPerPixel() const { return mBytesPerPixel; }

		void	setBytesPerPixel(uint bytesPerPixel){ mBytesPerPixel = bytesPerPixel; }

		void	randomChannelNoise(uint8 channel, uint8 minRange, uint8 maxRange);

		void	setColor(uint16 x, uint16 y, uint32 color);

		void	getColor(uint16 x, uint16 y, uint32& color) const;
	

	protected:
		void	freeData();

	protected:

		uchar*		mData;
		size_t		mWidth, mHeight;
		size_t		mSize;
		size_t		mDepth;	
		size_t		mBytesPerPixel;
		PixelFormat	mFormat;
	};

	typedef SharedPtr<Image>	ImagePtr;
	typedef vector<Image*>::type ImagePtrList;
	typedef vector<const Image*>::type ConstImagePtrList;
}

#endif