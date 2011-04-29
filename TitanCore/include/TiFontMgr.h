#ifndef __TITAN_FONT_MGR__HH
#define __TITAN_FONT_MGR__HH

#include "TiPrerequisites.h"
#include "TiFont.h"
#include "TiResourceMgr.h"
#include "TiSingleton.h"
#include "TiScriptLoader.h"


#include "rapidxml.hpp"

namespace Titan
{
	class _DllExport FontMgr :public ScriptLoader, public ResourceMgr, public Singleton<FontMgr>
	{
	public:
		FontMgr();

		~FontMgr();


		//scriptLoader methods
		virtual const StringVector& getScriptExts() const{ return mScriptExts; }

		virtual int getScriptLoaderPriority() const{ return mLoadOrder; }

		virtual void parseScript(DataStreamPtr& stream, const String& group);

	protected:
		Resource*	createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams);

		void	processXmlNode(rapidxml::xml_node<char>* xmlNode, const String&  group);

	protected:
		StringVector	mScriptExts;
	public:
		static FontMgr& getSingleton();
		static FontMgr* getSingletonPtr();
	};
}

#endif