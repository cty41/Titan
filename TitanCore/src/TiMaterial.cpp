#include "TitanStableHeader.h"
#include "TiMaterial.h"
#include "TiMaterialMgr.h"
#include "TiPass.h"

namespace Titan
{
	Material::Material(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual)
		:Resource(mgr, name, id, group, isManual)
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
	Pass*	Material::getPass(size_t index)
	{
		assert(index < mPassVec.size() && "invalid pass index");
		return mPassVec[index];
	}
	//-------------------------------------------------------------------------------//
	void Material::setDepthWrite(bool enable)
	{
		PassVec::iterator it = mPassVec.begin(), itend = mPassVec.end();
		for(;it != itend; ++it)
			(*it)->setDepthWrite(enable);
	}
	//------------------------------------------------------------------------------//
	void Material::setDefault()
	{
		//to do
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