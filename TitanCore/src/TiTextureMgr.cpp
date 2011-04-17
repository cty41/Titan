#include "TitanStableHeader.h"
#include "TiTexture.h"
#include "TiTextureMgr.h"

namespace Titan
{
	template<> TextureMgr* Singleton<TextureMgr>::ms_Singleton = 0;
	TextureMgr& TextureMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	TextureMgr* TextureMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}
	TextureMgr::TextureMgr()
		:ResourceMgr()
	{
		mLoadOrder = 10;
	}
	//-------------------------------------------------------------------------------//
	TextureMgr::~TextureMgr()
	{

	}
	//-------------------------------------------------------------------------------//
	TexturePtr TextureMgr::createManually(const String& name, const String& group, TexType type, 
		uint width, uint height, uint mipmapLevel,TexUsage usage, PixelFormat format, TexPool pool)
	{
		CreatedResource res  = create(name, group);
		TexturePtr tex = res.second;
		if(res.first)
		{
			tex->setTexType(type);
			tex->setWidth(width);
			tex->setHeight(height);
			tex->setMipmapLevel(mipmapLevel);
			tex->setTexUsage(usage);
			tex->setPixelFormat(format);
			tex->setTexPool(pool);
			tex->_createCoreObject();
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_WARNNING, 
				"There has a same named " + name +" texture existed, we just return old one in case of security",
				"TextureMgr::createManually");
		}
		return tex;
	}


}
