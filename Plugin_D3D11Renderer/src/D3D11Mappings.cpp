#include "D3D11Mappings.h"
#include "TiMatrix4.h"

namespace Titan
{

	//-------------------------------------------------------------------------------//
	D3D11_CULL_MODE D3D11Mappings::convertToD3D11(CullingMode cm)
	{
		switch( cm )
		{
		case CULL_NONE:
			return D3D11_CULL_NONE;
		case CULL_CLOCKWISE:
			return D3D11_CULL_BACK;
		case CULL_COUNTERCLOCKWISE:
			return D3D11_CULL_FRONT;
		}
		return D3D11_CULL_NONE;
	}
	//-------------------------------------------------------------------------------//
	D3D11_FILL_MODE D3D11Mappings::convertToD3D11(PolygonMode level)
	{
		switch(level)
		{
		case PM_POINTS:
		case PM_WIREFRAME:
			return D3D11_FILL_WIREFRAME;
		case PM_SOLID:
			return D3D11_FILL_SOLID;
		}
		return D3D11_FILL_SOLID;
	}
	//-------------------------------------------------------------------------------//
	D3D11_BLEND D3D11Mappings::convertToD3D11(SceneBlendFactor sbf)
	{
		switch( sbf )
		{
		case SBF_ONE:
			return D3D11_BLEND_ONE;
		case SBF_ZERO:
			return D3D11_BLEND_ZERO;
		case SBF_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;
		case SBF_SOURCE_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case SBF_ONE_MINUS_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case SBF_ONE_MINUS_SOURCE_COLOR:
			return D3D11_BLEND_INV_SRC_COLOR;
		case SBF_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case SBF_SOURCE_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case SBF_ONE_MINUS_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return D3D11_BLEND_INV_SRC_COLOR;
		}
		return D3D11_BLEND_ONE;
	}
	//-------------------------------------------------------------------------------//
	D3D11_BLEND_OP D3D11Mappings::convertToD3D11(SceneBlendOperation sbo)
	{
		switch(sbo)
		{
		case SBO_ADD:
			return D3D11_BLEND_OP_ADD;
		case SBO_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case SBO_REVERSE_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case SBO_MIN:
			return D3D11_BLEND_OP_MIN;
		case SBO_MAX:
			return D3D11_BLEND_OP_MAX;
		}

		return D3D11_BLEND_OP_ADD;
	}
	//----------------------------------------------------------------------------//
	D3D11_TEXTURE_ADDRESS_MODE D3D11Mappings::convertToD3D11(TexAddressMode tam)
	{
		switch(tam)
		{
		case TAM_WARP:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case TAM_BORDER:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case TAM_MIRROR:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case TAM_CLAMP:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		default: return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}
	//----------------------------------------------------------------------------//
	D3D11_FILTER  D3D11Mappings::convertToD3D11(FilterOptions fo)
	{
		switch (fo)
		{
		case FO_POINT:
		case FO_NONE:
		default:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case FO_LINEAR:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}
	//----------------------------------------------------------------------------//
	D3D11_COMPARISON_FUNC D3D11Mappings::convertToD3D11(CompareFunction cf)
	{
		switch( cf )
		{
		case CMPF_ALWAYS_FAIL:
			return D3D11_COMPARISON_NEVER;
		case CMPF_ALWAYS_PASS:
			return D3D11_COMPARISON_ALWAYS;
		case CMPF_LESS:
			return D3D11_COMPARISON_LESS;
		case CMPF_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL ;
		case CMPF_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case CMPF_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case CMPF_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL ;
		case CMPF_GREATER:
			return D3D11_COMPARISON_GREATER;
		};
		return D3D11_COMPARISON_LESS_EQUAL;
	}
	
	//change later..a messy..
	D3D11_USAGE D3D11Mappings::convertToD3D11(HardwareBuffer::Usage usage)
	{
		switch(usage)
		{
		case HardwareBuffer::HBU_STATIC:
		default:
			return D3D11_USAGE_DEFAULT;
		case HardwareBuffer::HBU_DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		}
	}

	D3D11_MAP D3D11Mappings::convertToD3D11(HardwareBuffer::LockOptions options, HardwareBuffer::Usage usage)
	{
		//todo, rewrite buffer option?
		return D3D11_MAP_WRITE_DISCARD;
	}

	LPCSTR D3D11Mappings::convertToD3D11(VertexElementSemantic sem)
	{
		switch (sem)	
		{
		case VES_BLEND_INDICES:
			return "BLENDINDICES";
			break;
		case VES_BLEND_WEIGHTS:
			return "BLENDWEIGHT";
			break;
		case VES_DIFFUSE:
			return "COLOR"; // NB index will differentiate
			break;
		case VES_SPECULAR:
			return "COLOR"; // NB index will differentiate
			break;
		case VES_NORMAL:
			return "NORMAL";
			break;
		case VES_POSITION:
			return "POSITION";
			break;
		case VES_TEXTURE_COORDINATES:
			return "TEXCOORD";
			break;
		case VES_BINORMAL:
			return "BINORMAL";
			break;
		case VES_TANGENT:
			return "TANGENT";
			break;
		}
		// to keep compiler happy
		return "POSITION";
	}

	DXGI_FORMAT D3D11Mappings::convertToD3D11(VertexElementType vType)
	{
		switch (vType)
		{
		case VET_COLOR:
		case VET_COLOR_ABGR:
		case VET_COLOR_ARGB:
			return DXGI_FORMAT_R32_UINT;
			break;
		case VET_FLOAT1:
			return DXGI_FORMAT_R32_FLOAT;
			break;
		case VET_FLOAT2:
			return DXGI_FORMAT_R32G32_FLOAT;
			break;
		case VET_FLOAT3:
			return DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case VET_FLOAT4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case VET_SHORT2:
			return DXGI_FORMAT_R16G16_SINT;
			break;
		case VET_SHORT4:
			return DXGI_FORMAT_R16G16B16A16_SINT;
			break;
		case VET_UBYTE4:
			return DXGI_FORMAT_R8G8B8A8_UINT;
			break;
		}
		// to keep compiler happy
		return DXGI_FORMAT_R32G32B32_FLOAT;
	}

	D3D10_PRIMITIVE_TOPOLOGY D3D11Mappings::convertToD3D11(OperationType ot)
	{
		switch(ot)
		{
		case OT_TRIANGLE_LIST:
			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case OT_TRIANGLE_STRIP:
			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		default:
			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	DXGI_FORMAT  D3D11Mappings::convertToD3D11(IndexBufferType ibt)
	{
		if(ibt == IBT_16Bits)
			return DXGI_FORMAT_R16_UINT;
		else
			return DXGI_FORMAT_R32_UINT;
	}
}