#include "TitanStableHeader.h"
#include "TiFontMgr.h"
#include "TiFont.h"
#include "TiResourceGroupMgr.h"


using namespace rapidxml;

namespace Titan
{
	template<> FontMgr* Singleton<FontMgr>::ms_Singleton = 0;
	FontMgr& FontMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	FontMgr* FontMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}

	FontMgr::FontMgr()
		:ResourceMgr()
	{
		mLoadOrder = 200;
		mScriptExts.push_back(".fnt");
		ResourceGroupMgr::getSingleton()._registerScriptLoader(this);
	}
	//-------------------------------------------------------------------------------//
	FontMgr::~FontMgr()
	{
		ResourceGroupMgr::getSingleton()._unregisterScriptLoader(this);
	}
	//-------------------------------------------------------------------------------//
	Resource* FontMgr::createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams)
	{
		return TITAN_NEW Font(this, name, id, group);
	}
	//------------------------------------------------------------------------------//
	void FontMgr::parseScript(DataStreamPtr& stream, const String& group)
	{
		MemoryDataStreamPtr memPtr = MemoryDataStreamPtr(TITAN_NEW MemoryDataStream(stream));
		xml_document<> doc;
		try
		{
			doc.parse<0>(reinterpret_cast<char*>(memPtr->getPtr()));
		}catch(parse_error pe)
		{
			String errMsg = pe.what();
			errMsg += pe.where<char>();
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				errMsg  ,
				"FontMgr::parseScript");
		}

		processXmlNode(doc.first_node(), group);
	}
	//------------------------------------------------------------------------------//
	void FontMgr::processXmlNode(rapidxml::xml_node<char>* xmlNode, const String& group)
	{
		if (strcmp(xmlNode->name(), "font") != 0)
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"font script does not support this kind of node: " + String(xmlNode->name()),
				"FontMgr::processXmlNode");
		}
		xml_attribute<char>* xmlAttribute =  xmlNode->first_attribute();
		if(xmlAttribute)
		{
			StringKeyValueMap keyValMap;
			while(xmlAttribute)
			{
				keyValMap.insert(StringKeyValueMap::value_type(xmlAttribute->name(), xmlAttribute->value()));
				xmlAttribute = xmlAttribute->next_attribute();
			}
			StringKeyValueMap::iterator it = keyValMap.find("name"), itend = keyValMap.end();
			if(it != itend)
			{
				FontPtr pFont = create(it->second, group).second;
				keyValMap.erase(it);
				pFont->setParams(keyValMap);
			}
		}
	}
}