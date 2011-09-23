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
	//I do not hanle error input, so be aware of that
	void extractFloats(const String& val, float* pOut, uint count)
	{
		size_t firstIdx = 0, idx = 0;
		while(count)
		{
			idx = val.find(' ', firstIdx) ;
			if(idx == String::npos)
				break;

			StringStream stream;
			String sub = val.substr(firstIdx, idx - firstIdx);
			stream << sub;
			stream >> *pOut++;
			firstIdx = idx + 1;
			--count;
		}
	}
	//------------------------------------------------------------------------------//
	void extractInts(const String& val, int* pOut, uint count)
	{
		size_t firstIdx = 0, idx = 0;
		while(count)
		{
			idx = val.find(' ', firstIdx) ;
			if(idx == String::npos)
				break;

			StringStream stream;
			String sub = val.substr(firstIdx, idx - firstIdx);
			stream << sub;
			stream >> *pOut++;
			firstIdx = idx + 1;
			--count;
		}
	}

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
			TITAN_EXCEPT_ITEMLOST(
				"We can not locate the name of the material"
				);
		}
		ResourceMgr::CreatedResource cr = MaterialMgr::getSingleton().create(it->second, group);
		if(!cr.first)
		{
			TITAN_EXCEPT_INTERNALERROR(
				"We have created a same name material: " + it->second
				);
			return ;
		}
		MaterialPtr pMaterial= cr.second;
		pMaterial->removeAllPasses();
		pScriptNode->createObj = pMaterial.get();
		
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
			TITAN_EXCEPT_ITEMLOST(
				"we could not find name node"
				);
			return ;
		}

		it = pScriptNode->keyValueMap.find("val");
		if(it != pScriptNode->keyValueMap.end())
		{
			paramVal = it->second;
		}
		else
		{
			TITAN_EXCEPT_ITEMLOST(
				"we could not find val node"
				);
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
			tu->setTexture(it->second);
			pScriptNode->keyValueMap.erase(it);
		}
		else
		{
			it = pScriptNode->keyValueMap.find("cube_texture");
			if(it != itend)
			{
				tu->setTexture(it->second, TT_CUBEMAP);
				pScriptNode->keyValueMap.erase(it);
			}
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
			TITAN_EXCEPT_ITEMLOST(
				"The vertex shader must have name attribute"
				);
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
				TITAN_EXCEPT_ITEMLOST(
					"auto_named_param does not have a name "
					);
				return ;
			}
			name = it->second;
			it = pScriptNode->keyValueMap.find("annotation");
			if (it == pScriptNode->keyValueMap.end())
			{
				TITAN_EXCEPT_ITEMLOST(
					"auto_named_param does not have a annotation "
					);
				return ;
			}
			const ShaderParams::AutoConstantDef* def = ShaderParams::getAutoConstantDef(it->second);
			if(!def)
			{
				TITAN_EXCEPT_ITEMLOST(
					"we do not have this type of auto_named_param: " + it->second
					);
			}

			if(def->edType == ShaderParams::EDT_NONE)
				params->setNamedAutoConstant(name, def->constantType);
			else 
			{
				it = pScriptNode->keyValueMap.find("extra_data");
				if (it == pScriptNode->keyValueMap.end())
				{
					TITAN_EXCEPT_ITEMLOST(
						"this auto_named_param need extra_data "
						);
					return ;
				}
				if(def->edType == ShaderParams::EDT_INT)
				{
					size_t extraData = StringConverter::parseUint(it->second);
					params->setNamedAutoConstant(name, def->constantType, extraData);
				}
				else
				{
					float extraData = StringConverter::parseFloat(it->second);
					params->setNamedAutoConstant(name, def->constantType, extraData);
				}
			}
		}
		else if(pScriptNode->name == "named_param")
		{
			String name, type;
			StringKeyValueMap::iterator it = pScriptNode->keyValueMap.find("name");
			if (it == pScriptNode->keyValueMap.end())
			{
				TITAN_EXCEPT_ITEMLOST(
					"auto_named_param does not have a name "
					);
				return ;
			}
			name = it->second;
			it = pScriptNode->keyValueMap.find("type");
			if (it == pScriptNode->keyValueMap.end())
			{
				TITAN_EXCEPT_ITEMLOST(
					"auto_named_param does not have type "
					);
				return ;
			}
			type = it->second;
			uint count = 0;
			bool isFLoat = false;
			if(type.find("float") != String::npos)
			{
				isFLoat = true;

				if (type.size() >= 6)
				{
					count = StringConverter::parseInt(type.substr(5));
				}
				else
					count = 1;
			}
			else if(type.find("int") != String::npos)
			{
				isFLoat = false;
				if (type.size() >= 4)
					count = StringConverter::parseInt(type.substr(3));
				else
					count = 1;
			}
			int roundedCount = count%4 != 0 ? count + 4 - (count%4) : count;

			bool hasVal = true;
			it = pScriptNode->keyValueMap.find("val");
			if (it == pScriptNode->keyValueMap.end())
			{
				hasVal = false;
			}
			String val = it->second;
			if(isFLoat)
			{
				float*	pFloat = TITAN_ALLOC_T(float, roundedCount, MEMCATEGORY_GENERAL);
				extractFloats(val, pFloat, roundedCount);
				params->setNamedConstant(name,pFloat, roundedCount, 1);
				TITAN_FREE(pFloat, MEMCATEGORY_GENERAL);
			}
			else
			{
				int* pInt = TITAN_ALLOC_T(int, roundedCount, MEMCATEGORY_GENERAL);
				extractInts(val, pInt, roundedCount);
				params->setNamedConstant(name, pInt, roundedCount, 1);
				TITAN_FREE(pInt, MEMCATEGORY_GENERAL);
			}


		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"we do not have this kind of shader param type: "+ pScriptNode->name
				);
		}
		
	}

}