#include "D3D11ShaderMgr.h"
#include "D3D11HLSLShader.h"
#include "TiException.h"

namespace Titan
{
	D3D11ShaderMgr::D3D11ShaderMgr()
		:ShaderMgr()
	{

	}
	//----------------------------------------------------------------------------//
	D3D11ShaderMgr::~D3D11ShaderMgr()
	{

	}
	//----------------------------------------------------------------------------//
	Resource* D3D11ShaderMgr::createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams)
	{
		AnyMap::const_iterator ait;
		if(!extraParams || (ait = extraParams->find("type")) == extraParams->end())
		{
			TITAN_EXCEPT_ITEMLOST(
				"we can not find shader type param");
			return NULL;
		}
		else
		{
			ShaderType type = any_cast<ShaderType>(ait->second);
			if (type == ST_VERTEX_SHADER)
			{
				return TITAN_NEW D3D11VertexShader(this,name, id, group, isManual);
			}
			else if(type == ST_PIXEL_SHADER)
			{
				return TITAN_NEW D3D11PixelShader(this, name, id, group, isManual);
			}
		}
		return NULL;
	}
	//------------------------------------------------------------------------------//
	Resource* D3D11ShaderMgr::createImpl(const String& name, ResourceHandle id, const String& group,ShaderType type)
	{
		if (type == ST_VERTEX_SHADER)
		{
			return TITAN_NEW D3D11VertexShader(this, name, id, group, false);
		}
		else if(type == ST_PIXEL_SHADER)
		{
			return TITAN_NEW D3D11PixelShader(this, name, id, group, false);
		}
		return NULL;
		//shut compiler up
	}
}