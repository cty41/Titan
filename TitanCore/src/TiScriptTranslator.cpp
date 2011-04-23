#include "TitanStableHeader.h"
#include "TiScriptTranslator.h"
#include "TiMaterialMgr.h"
#include "TiScriptTranslatorMgr.h"
#include "TiPass.h"
#include "TiShaderMgr.h"
#include "TiShaderParams.h"
#include "TiTextureMgr.h"

namespace Titan
{

	typedef ScriptCompilerMgr::ScriptNodePtrList ScriptNodePtrList;

	ScriptTranslator::ScriptTranslator(ScriptTranslatorMgr* mgr)
		:mMgr(mgr)
	{
	}
	//------------------------------------------------------------------------------//
	void ScriptTranslator::translate(ScriptNodePtr& pScriptNode, const String& group)
	{
		ScriptNodePtrList::iterator nit = pScriptNode->children.begin(), nitEnd = pScriptNode->children.end();
		while(nit != nitEnd)
		{
			ScriptTranslator* pTranslator = mMgr->getTranslator((*nit)->name);
			if (pTranslator)
			{
				pTranslator->translate(*nit, group);
			}
			++nit;
		}
	}



	//
	//MaterialTranslator
	//
	MaterialTranslator::MaterialTranslator(ScriptTranslatorMgr* mgr)
		:ScriptTranslator(mgr), mName("material")
	{
		mType = STT_MATERIAL;
	}
	//------------------------------------------------------------------------------//
	void MaterialTranslator::translate(ScriptNodePtr& pScriptNode, const String& group)
	{
		StringKeyValueMap::iterator it = pScriptNode->keyValueMap.find("name"), itend = pScriptNode->keyValueMap.end();
		if(it == itend)
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"We can not locate the name of the material",
				"MaterialTranslator::translate");
		}
		
		Material* pMaterial= reinterpret_cast<Material*>(MaterialMgr::getSingleton().create(it->second, group).second.get());
		pMaterial->removeAllPasses();
		pScriptNode->createObj = pMaterial;
		
		ScriptTranslator::translate(pScriptNode, group);
	}



	//
	//PassTranslator
	//
	PassTranslator::PassTranslator(ScriptTranslatorMgr* mgr)
		:ScriptTranslator(mgr), mName("pass")
	{
		mType = STT_PASS;
	}
	//------------------------------------------------------------------------------//
	void PassTranslator::translate(ScriptNodePtr& pScriptNode, const String& group)
	{
		Material* pMaterial = any_cast<Material*>(pScriptNode->parent->createObj);
		Pass* pass = pMaterial->createPass();
		pScriptNode->createObj = pass;
		
		//set Pass settings, to do
		ScriptNodePtrList::iterator nit = pScriptNode->children.begin(), nitEnd = pScriptNode->children.end();
		while(nit != nitEnd)
		{
			if ((*nit)->name == "render_state")
			{
				translateRenderState(*nit, pass);
				nit = pScriptNode->children.erase(nit);
				continue;
			}
			else if((*nit)->name == "vertex_shader")
			{
				translateShader(*nit, group);
				nit = pScriptNode->children.erase(nit);
				continue;
			}
			else if((*nit)->name == "pixel_shader")
			{
				translateShader(*nit, group);
				nit = pScriptNode->children.erase(nit);
				continue;
			}
			++nit;
		}

		ScriptTranslator::translate(pScriptNode, group);
	}
	//------------------------------------------------------------------------------//
	void PassTranslator::translateRenderState(ScriptNodePtr& pScriptNode, Pass* parent)
	{
		String paramName, paramVal;
		StringKeyValueMap::iterator it = pScriptNode->keyValueMap.find("name");
		if(it != pScriptNode->keyValueMap.end())
		{
			paramName = it->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"we could not find name node",
				"PassTranslator::translateRenderState");
			return ;
		}

		it = pScriptNode->keyValueMap.find("val");
		if(it != pScriptNode->keyValueMap.end())
		{
			paramVal = it->second;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"we could not find val node",
				"PassTranslator::translateRenderState");
			return ;
		}

		parent->setSingleParam(paramName, paramVal);
	}
	//------------------------------------------------------------------------------//
	void PassTranslator::translateShader(ScriptNodePtr& pScriptNode, const String& group)
	{
		ScriptTranslator* pTranslator = mMgr->getTranslator("shader");
		pTranslator->translate(pScriptNode, group);
	}


	//
	//TextureUnitTranslator
	//
	TextureUnitTranslator::TextureUnitTranslator(ScriptTranslatorMgr* mgr)
		:ScriptTranslator(mgr), mName("texture_unit")
	{
		mType = STT_TEXTURE_UNIT;
	}
	//------------------------------------------------------------------------------//
	void TextureUnitTranslator::translate(ScriptNodePtr& pScriptNode, const String& group)
	{
		Pass* pass = any_cast<Pass*>(pScriptNode->parent->createObj);
		TextureUnit* tu = pass->createTextureUnit();
		pScriptNode->createObj = tu;

		//set TextureUnit setting
		StringKeyValueMap::iterator it = pScriptNode->keyValueMap.find("texture"), itend = pScriptNode->keyValueMap.end();
		if(it != itend)
		{
			TexturePtr pTex = TextureMgr::getSingleton().create(it->second, group).second;
			tu->setTexture(pTex);
			pScriptNode->keyValueMap.erase(it);
		}
		
		tu->setParams(pScriptNode->keyValueMap);
	}


	//
	//ShaderTranslator
	//
	ShaderTranslator::ShaderTranslator(ScriptTranslatorMgr* mgr)
		:ScriptTranslator(mgr), mName("shader")
	{
		mType = STT_SHADER;
	}
	//------------------------------------------------------------------------------//
	void ShaderTranslator::translate(ScriptNodePtr& pScriptNode, const String& group)
	{
		ShaderType st;
		if (pScriptNode->name == "vertex_shader")
		{
			st = ST_VERTEX_SHADER;
		}
		else if (pScriptNode->name == "pixel_shader")
		{
			st = ST_PIXEL_SHADER;
		}

		StringKeyValueMap::iterator it = pScriptNode->keyValueMap.find("name");
		if(it == pScriptNode->keyValueMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"The vertex shader must have name attribute",
				"ShaderTranslator::translate");
			return;
		}
		String shaderName = it->second;
		ShaderPtr pShader = ShaderMgr::getSingleton().create(shaderName, group, st);
		pScriptNode->keyValueMap.erase(it);
		//set shader attributes
		pShader->setParams(pScriptNode->keyValueMap);
		ShaderParamsPtr pParams = pShader->getShaderParams();
		ScriptNodePtrList::iterator sit = pScriptNode->children.begin(), sitEnd = pScriptNode->children.end();
		while (sit != sitEnd)
		{
			translateShaderParam(*sit, pParams);
			++sit;
		}
		Pass* parent = any_cast<Pass*>(pScriptNode->parent->createObj);
		if(st == ST_VERTEX_SHADER)
		{
			parent->setVertexShader(shaderName);
		}
		else if(st == ST_PIXEL_SHADER)
		{
			parent->setPixelShader(shaderName);
		}
		

	}
	//------------------------------------------------------------------------------//
	void ShaderTranslator::translateShaderParam(ScriptNodePtr& pScriptNode, ShaderParamsPtr& params)
	{
		if(pScriptNode->name == "auto_named_param")
		{
			String name;
			StringKeyValueMap::iterator it = pScriptNode->keyValueMap.find("name");
			if (it == pScriptNode->keyValueMap.end())
			{
				TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
					"auto_named_param does not have a name ",
					"ShaderTranslator::translateShaderParam");
				return ;
			}
			name = it->second;
			it = pScriptNode->keyValueMap.find("annotation");
			if (it == pScriptNode->keyValueMap.end())
			{
				TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
					"auto_named_param does not have a annotation ",
					"ShaderTranslator::translateShaderParam");
				return ;
			}
			const ShaderParams::AutoConstantDef* def = ShaderParams::getAutoConstantDef(it->second);
			if(!def)
			{
				TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
					"we do not have this type of auto_named_param: " + it->second,
					"ShaderTranslator::translateShaderParam");
			}
			params->setNamedAutoConstant(name, def->constantType);
		}
		else if(pScriptNode->name == "named_param")
		{

		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"we do not have this kind of shader param type: "+ pScriptNode->name,
				"ShaderTranslator::translateShaderParam");
		}
	}

}