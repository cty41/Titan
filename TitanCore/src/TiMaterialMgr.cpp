#include "TitanStableHeader.h"
#include "TiMaterialMgr.h"
#include "TiResourceGroupMgr.h"

namespace Titan
{

	template<> MaterialMgr* Singleton<MaterialMgr>::ms_Singleton = 0;
	MaterialMgr& MaterialMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	MaterialMgr* MaterialMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}

	MaterialMgr::MaterialMgr()
		:mDefaultMinFilter(FO_POINT), mDefaultMaxFilter(FO_POINT), 
		mDefaultMipFilter(FO_NONE)
	{
		mLoadOrder = 100;
		createDefaultMaterial();
	}
	//----------------------------------------------------------------------------//
	MaterialMgr::~MaterialMgr()
	{

	}
	//----------------------------------------------------------------------------//
	Resource*	MaterialMgr::createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams)
	{
		return TITAN_NEW Material(this, name, id, group);
	}
	//----------------------------------------------------------------------------//
	void MaterialMgr::createDefaultMaterial()
	{
		mDefaultMaterial = create("DefaultMaterial", ResourceGroupMgr::INTERNAL_RESOURCE_GROUP).second;
		mDefaultMaterial->createPass()->createTextureUnit();
	}
}