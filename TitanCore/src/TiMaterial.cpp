#include "TitanStableHeader.h"
#include "TiMaterial.h"
#include "TiMaterialMgr.h"
#include "TiPass.h"

namespace Titan
{
	Material::Material(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:Resource(mgr, name, id, group)
	{
		setDefault();
	}
	//-------------------------------------------------------------------------------//
	Material::~Material()
	{
		removeAllPasses();
	}
	//-------------------------------------------------------------------------------//
	Pass* Material::createPass()
	{
		Pass* p = TITAN_NEW Pass(this, mPassVec.size());
		mPassVec.push_back(p);
		return p;
	}
	//-------------------------------------------------------------------------------//
	void Material::removeAllPasses()
	{
		PassVec::iterator it = mPassVec.begin(), itend = mPassVec.end();
		for(;it != itend; ++it)
		{
			TITAN_DELETE (*it);
		}
		mPassVec.clear();
	}
	//-------------------------------------------------------------------------------//

	//-------------------------------------------------------------------------------//
	void Material::setDefault()
	{
		//to do
	}
	//-------------------------------------------------------------------------------//
	void Material::prepareImpl()
	{
		
	}
	//-------------------------------------------------------------------------------//
	void Material::unprepareImpl()
	{

	}
	//-------------------------------------------------------------------------------//
	void Material::loadImpl()
	{
		PassVec::iterator it = mPassVec.begin(), itend = mPassVec.end();
		for(;it != itend; ++it)
			(*it)->_load();
	}
	//-------------------------------------------------------------------------------//
	void Material::unloadImpl()
	{
		PassVec::iterator it = mPassVec.begin(), itend = mPassVec.end();
		for(;it != itend; ++it)
			(*it)->_unload();
	}
}