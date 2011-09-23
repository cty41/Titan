#include "TitanStableHeader.h"
#include "TiScriptTranslatorMgr.h"

namespace Titan
{
	ScriptTranslatorMgr::ScriptTranslatorMgr()
	{
		addTranslator(TITAN_NEW MaterialTranslator(this));
		addTranslator(TITAN_NEW PassTranslator(this));
		addTranslator(TITAN_NEW TextureUnitTranslator(this));
		addTranslator((TITAN_NEW ShaderTranslator(this)));
	}
	//------------------------------------------------------------------------------//
	ScriptTranslatorMgr::~ScriptTranslatorMgr()
	{
		removeAllTranslator();
	}
	//------------------------------------------------------------------------------//
	void ScriptTranslatorMgr::addTranslator(ScriptTranslator* translator)
	{
		ScriptTranslatorMap::iterator it = mScriptTranslatorMap.find(translator->getName());
		if(it != mScriptTranslatorMap.end())
		{
			TITAN_EXCEPT_INTERNALERROR(
				"This kind of ScriptTranslator has existed!: " + translator->getName()
				);
		}
		else
		{
			mScriptTranslatorMap.insert(ScriptTranslatorMap::value_type(translator->getName(), translator));

		}
	}
	//------------------------------------------------------------------------------//
	void ScriptTranslatorMgr::removeAllTranslator()
	{
		ScriptTranslatorMap::iterator it = mScriptTranslatorMap.begin(), itend = mScriptTranslatorMap.end();
		while(it != itend)
		{
			TITAN_DELETE it->second;
			++it;
		}
		mScriptTranslatorMap.clear();
	}
	//------------------------------------------------------------------------------//
	ScriptTranslator* ScriptTranslatorMgr::getTranslator(const String&  name)
	{
		ScriptTranslatorMap::iterator it = mScriptTranslatorMap.find(name);
		if(it != mScriptTranslatorMap.end())
		{
			return it->second;
		}
		else
		{
			TITAN_EXCEPT_ITEMLOST(
				"we can not find this type of ScriptTranslator: " + name
				);
			return NULL;
		}
	}
	//------------------------------------------------------------------------------//
	void ScriptTranslatorMgr::translate(ScriptNodePtr& nodePtr, const String& group)
	{
		//we have ensure the name can be found earlier
		ScriptTranslatorMap::iterator it = mScriptTranslatorMap.find(nodePtr->name);
		it->second->translate(nodePtr, group);
	}
}