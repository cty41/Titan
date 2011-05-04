#include "TitanStableHeader.h"
#include "TiScriptCompilerMgr.h"
#include "TiResourceGroupMgr.h"
#include "TiDataStream.h"
#include "TiScriptTranslatorMgr.h"


using namespace rapidxml;

namespace Titan
{
	template<> ScriptCompilerMgr* Singleton<ScriptCompilerMgr>::ms_Singleton = 0;

	ScriptCompilerMgr* ScriptCompilerMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}
	ScriptCompilerMgr& ScriptCompilerMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	ScriptCompilerMgr::ScriptCompilerMgr()
	{
		mFileNameExts.push_back(".mtl");
		ResourceGroupMgr::getSingleton()._registerScriptLoader(this);

		mScriptTranslatorMgr = TITAN_NEW ScriptTranslatorMgr();
	}
	//------------------------------------------------------------------------------//
	ScriptCompilerMgr::~ScriptCompilerMgr()
	{
		ResourceGroupMgr::getSingleton()._unregisterScriptLoader(this);
		TITAN_DELETE mScriptTranslatorMgr;
	}
	//------------------------------------------------------------------------------//
	void ScriptCompilerMgr::parseScript(DataStreamPtr& stream, const String& group)
	{
		String scriptContent, line;
		while (!stream->eof())
		{
			line = stream->getLine();
			if(!line.length() == 0)
				scriptContent.append(line);
		}
		xml_document<> doc;
		try
		{
			doc.parse<0>(&scriptContent[0]);
		}catch(parse_error pe)
		{
			String errMsg = pe.what();
			errMsg += "happened in: ";
			errMsg.append(pe.where<char>());
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				errMsg  ,
				"ScriptCompilerMgr::parseScript");
		}
		processXmlNode(doc.first_node(), NULL);

		ScriptRootNodePtrVec::iterator it = mScriptRootNodePtrVec.begin(), itend = mScriptRootNodePtrVec.end();
		while(it != itend)
		{
			mScriptTranslatorMgr->translate(*it, group);
			++it;
		}

		//free all ScriptNode;
		mScriptRootNodePtrVec.clear();
	}
	//------------------------------------------------------------------------------//
	void ScriptCompilerMgr::processXmlNode(xml_node<char>* xmlNode, ScriptNode* parent)
	{
		//here we do not delete ScriptNode manually, because the sharedPtr will be in charge of this
		ScriptNode* pNode = TITAN_NEW ScriptNode(parent);

		if (parent == NULL)
		{
			
			ScriptTranslator* pTranslator = mScriptTranslatorMgr->getTranslator(xmlNode->name());
			if (pTranslator == NULL)
			{
				String errMsg = "Root node with this type :" ;
				TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
					errMsg + xmlNode->name() + "does not existed!",
					"ScriptCompilerMgr::processXmlNode");
				return ;
			}
			
			//set attributes
			pNode->name = xmlNode->name();
			xml_attribute<>* xmlAttribute = xmlNode->first_attribute();

			while (xmlAttribute)
			{
				pNode->keyValueMap.insert(StringKeyValueMap::value_type(xmlAttribute->name(), xmlAttribute->value()));
				xmlAttribute = xmlAttribute->next_attribute();
			}
			mScriptRootNodePtrVec.push_back(ScriptNodePtr(pNode));
		}
		else
		{
			//set attributes
			pNode->name = xmlNode->name();
			xml_attribute<>* xmlAttribute = xmlNode->first_attribute();

			while (xmlAttribute)
			{
				pNode->keyValueMap.insert(StringKeyValueMap::value_type(xmlAttribute->name(), xmlAttribute->value()));
				xmlAttribute = xmlAttribute->next_attribute();
			}
			parent->children.push_back(ScriptNodePtr(pNode));
		}

		//recursive process every child node
		xml_node<>* first_child = xmlNode->first_node();
		if(first_child)
			processXmlNode(first_child, pNode);
		//recursive process every sibling node
		xml_node<>* first_sibling = xmlNode->next_sibling();
		if (first_sibling)
			processXmlNode(first_sibling, parent);
		
	}

}