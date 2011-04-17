#include "TitanStableHeader.h"
#include "TitanFontMgr.h"
#include "TitanFont.h"

namespace Titan
{
	template<> FontMgr* Singleton<FontMgr>::ms_Singleton = 0;
	FontMgr& FontMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	FontMgr* FontMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}

	FontMgr::FontMgr()
		:ResourceMgr()
	{
		mLoadOrder = 200;
	}
	//-------------------------------------------------------------------------------//
	FontMgr::~FontMgr()
	{

	}
	//-------------------------------------------------------------------------------//
	Resource* FontMgr::createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams)
	{
		return TITAN_NEW Font(this, name, id, group);
	}
}