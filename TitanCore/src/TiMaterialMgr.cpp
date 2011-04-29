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
		//set for scriptLoader
		mLoadOrder = 100;
		mScriptExts.push_back(".mtl");

		
		//ResourceGroupMgr::getSingleton()._registerScriptLoader(this);


		createDefaultMaterial();
	}
	//----------------------------------------------------------------------------//
	MaterialMgr::~MaterialMgr()
	{
		//ResourceGroupMgr::getSingleton()._unregisterScriptLoader(this);
	}
	//------------------------------------------------------------------------------//
	void MaterialMgr::parseScript(DataStreamPtr& stream, const String& group)
	{

	}
	//----------------------------------------------------------------------------//
	Resource*	MaterialMgr::createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams)
	{
		return TITAN_NEW Material(this, name, id, group, isManual);
	}
	//----------------------------------------------------------------------------//
	void MaterialMgr::createDefaultMaterial()
	{
		mDefaultMaterial = create("DefaultMaterial", ResourceGroupMgr::INTERNAL_RESOURCE_GROUP).second;
		mDefaultMaterial->createPass()->createTextureUnit();
	}

}