#include "TitanStableHeader.h"
#include "TiTexture.h"

namespace Titan
{
	Texture::Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual)
		: Resource(mgr, name, id, group, isManual), mType(TT_2D), mMipmapsLevel(1), mPixelFormat(PF_UNKNOWN), mWidth(0),
		mHeight(0), mDepth(0)
	{
	}
	//-------------------------------------------------------------------------------//
	Texture::~Texture()
	{
	}

	void Texture::prepareImpl()
	{
		if (isManualLoaded())
			return;

		DataStreamPtr dataStream = ResourceGroupMgr::getSingltonPtr()->openResource(mName, mGroup);
		mPreparedData = MemoryDataStreamPtr(TITAN_NEW MemoryDataStream(dataStream));
	}

	void Texture::unprepareImpl()
	{
		mPreparedData.setNull();
	}
	//------------------------------------------------------------------------------//
	void Texture::loadImage(const Image& img)
	{
		if(mLoadState != LS_UNPREPARED)
			return ;

		vector<const Image*>::type imagePtrs;
		imagePtrs.push_back(&img);
		_loadImages( imagePtrs );

		mLoadState = LS_LOADED;
	}
	//-------------------------------------------------------------------------------//
	void Texture::generatePerlinNoise(float scale, int octaves, float falloff)
	{
		if(mType == TT_2D)
		{
			_perlinNoiseImpl(scale, octaves, falloff);
		}
		else
		{
			TITAN_EXCEPT_INTERNALERROR(
				"Perlin noise can only made in 2d texture!"
				);
		}

	}
	//-------------------------------------------------------------------------------//
	bool Texture::hasAlpha() const 
	{
		return PixelFuncs::hasAlpha(mPixelFormat);
	}
	//------------------------------------------------------------------------------//
	void Texture::_loadImages(const ConstImagePtrList& images)
	{
		if(images.size() < 1)
			TITAN_EXCEPT_INVALIDPARAMS(
			"image list is empty"
			);

		mWidth = images[0]->getWidth();
		mHeight = images[0]->getHeight();

		mPixelFormat = images[0]->getFormat();

		_loadImgsImpl(images);
	}
	//------------------------------------------------------------------------------//


}