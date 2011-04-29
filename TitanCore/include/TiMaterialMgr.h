#ifndef _TITAN_MATERIAL_MGR__HH
#define _TITAN_MATERIAL_MGR__HH

#include "TiPrerequisites.h"
#include "TiResourceMgr.h"
#include "TiMaterial.h"
#include "TiSingleton.h"
#include "TiScriptLoader.h"


namespace Titan
{
	class _DllExport MaterialMgr : public ResourceMgr, public ScriptLoader, public Singleton<MaterialMgr>
	{
	public:
		MaterialMgr();

		~MaterialMgr();

		void	createDefaultMaterial();

		MaterialPtr	getDefaultMaterial() const { return mDefaultMaterial;}

		//ScriptLoader methods, no use now
		const StringVector& getScriptExts() const{ return mScriptExts; }

		int getScriptLoaderPriority() const { return mLoadOrder; }

		void parseScript(DataStreamPtr& stream, const String& group);
	
	protected:
		Resource*	createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams);

	protected:
		FilterOptions	mDefaultMinFilter;
		FilterOptions	mDefaultMaxFilter;
		FilterOptions	mDefaultMipFilter;
		MaterialPtr		mDefaultMaterial;

		StringVector	mScriptExts;

	public:
		static MaterialMgr& getSingleton();
		static MaterialMgr*	getSingletonPtr();

	};
}

#endif