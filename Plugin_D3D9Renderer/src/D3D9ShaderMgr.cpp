#include "D3D9ShaderMgr.h"
#include "TiException.h"
#include "D3D9Shader.h"


namespace Titan
{
	D3D9ShaderMgr::D3D9ShaderMgr()
		:ShaderMgr()
	{

	}
	//----------------------------------------------------------------------------//
	D3D9ShaderMgr::~D3D9ShaderMgr()
	{

	}
	//----------------------------------------------------------------------------//
	Resource* D3D9ShaderMgr::createImpl(const String& name, ResourceHandle id, const String& group, AnyMap* extraParams)
	{
		AnyMap::const_iterator ait;
		if(!extraParams || (ait = extraParams->find("type")) == extraParams->end())
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"we can not find shader type param",
				"D3D9ShaderMgr::createImpl");
			return NULL;
		}
		else
		{
			ShaderType type = any_cast<ShaderType>(ait->second);
			if (type == ST_VERTEX_SHADER)
			{
				return TITAN_NEW D3D9VertexShader(this,name, id, group);
			}
			else if(type == ST_PIXEL_SHADER)
			{
				return TITAN_NEW D3D9PixelShader(this, name, id, group);
			}
		}
		return NULL;
	}
	//------------------------------------------------------------------------------//
	Resource* D3D9ShaderMgr::createImpl(const String& name, ResourceHandle id, const String& group,ShaderType type)
	{
		if (type == ST_VERTEX_SHADER)
		{
			return TITAN_NEW D3D9VertexShader(this,name, id, group);
		}
		else if(type == ST_PIXEL_SHADER)
		{
			return TITAN_NEW D3D9PixelShader(this, name, id, group);
		}
		return NULL;
		//shut compiler up
	}
}