#ifndef __TITAN_SCRIPT_COMPILER_MGR_HH
#define __TITAN_SCRIPT_COMPILER_MGR_HH

#include "TiPrerequisites.h"
#include "TiScriptLoader.h"
#include "TiSingleton.h"
#include "TiSharedPtr.h"
#include "TiAny.h"

#include "rapidxml.hpp"

namespace Titan
{
	class _DllExport ScriptCompilerMgr : public GeneralAlloc, public ScriptLoader, public Singleton<ScriptCompilerMgr>
	{
	public:
		struct ScriptNode;
		typedef SharedPtr<ScriptNode> ScriptNodePtr;
		typedef list<ScriptNodePtr>::type ScriptNodePtrList;
		typedef	vector<ScriptNodePtr>::type ScriptRootNodePtrVec;


		struct ScriptNode : public GeneralAlloc
		{
			String		name;
			ScriptNode* parent;	//also use sharedPtr?
			ScriptNodePtrList	children;

			StringKeyValueMap	keyValueMap;
			Any			createObj;

			ScriptNode(ScriptNode* parent): parent(parent){}
		};
	public:
		ScriptCompilerMgr();

		~ScriptCompilerMgr();

		//ScriptLoader methods
		virtual const StringVector& getScriptExts() const{ return mFileNameExts; }

		virtual int getScriptLoaderPriority() const { return 90; };

		virtual void parseScript(DataStreamPtr& stream, const String& group);

		void processXmlNode(rapidxml::xml_node<char>* xmlNode, ScriptNode* parent);



	protected:
		ScriptTranslatorMgr*	mScriptTranslatorMgr;
		StringVector	mFileNameExts;
		ScriptRootNodePtrVec	mScriptRootNodePtrVec;

	public:
		static ScriptCompilerMgr*	getSingletonPtr();

		static ScriptCompilerMgr&	getSingleton();
	};
}
#endif