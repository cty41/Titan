#include "TitanStableHeader.h"
#include "TitanTexture.h"
#include "TitanTextureMgr.h"

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

	}
	//-------------------------------------------------------------//
	TextureMgr::~TextureMgr()
	{

	}
	//-------------------------------------------------------------//


}
