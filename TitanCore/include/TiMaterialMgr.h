#ifndef _TITAN_MATERIAL_MGR__HH
#define _TITAN_MATERIAL_MGR__HH

#include "TiPrerequisites.h"
#include "TiResourceMgr.h"
#include "TiMaterial.h"
#include "TiSingleton.h"

namespace Titan
{
	class _DllExport MaterialMgr : public ResourceMgr, public Singleton<MaterialMgr>
	{
	public:
		MaterialMgr();

		~MaterialMgr();

		void	createDefaultMaterial();

		MaterialPtr	getDefaultMaterial() const { return mDefaultMaterial;}
	
	protected:
		Resource*	createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams);

	protected:
		FilterOptions	mDefaultMinFilter;
		FilterOptions	mDefaultMaxFilter;
		FilterOptions	mDefaultMipFilter;
		MaterialPtr		mDefaultMaterial;

	public:
		static MaterialMgr& getSingleton();
		static MaterialMgr*	getSingletonPtr();

	};
}

#endif