#include "D3D9Mappings.h"
#include "TiMatrix4.h"

namespace Titan
{
	//-------------------------------------------------------------------------------//
	DWORD D3D9Mappings::convertToD3D9(ShadeOptions so)
	{
		switch( so )
		{
		case SO_FLAT:
			return D3DSHADE_FLAT;
		case SO_GOURAUD:
			return D3DSHADE_GOURAUD;
		case SO_PHONG:
			return D3DSHADE_PHONG;
		}
		return 0;
	}
	//-------------------------------------------------------------------------------//
	DWORD D3D9Mappings::convertToD3D9(CullingMode cm, bool flip)
	{
		switch( cm )
		{
		case CULL_NONE:
			return D3DCULL_NONE;
		case CULL_CLOCKWISE:
			if( flip )
				return D3DCULL_CCW;
			else
				return D3DCULL_CW;
		case CULL_COUNTERCLOCKWISE:
			if( flip )
				return D3DCULL_CW;
			else
				return D3DCULL_CCW;
		}
		return 0;
	}
	//-------------------------------------------------------------------------------//
	D3DFOGMODE D3D9Mappings::convertToD3D9(FogMode fm)
	{
		switch( fm )
		{
		case FOG_EXP:
			return D3DFOG_EXP;
		case FOG_EXP2:
			return D3DFOG_EXP2;
		case FOG_LINEAR:
			return D3DFOG_LINEAR;
		}
		return D3DFOG_FORCE_DWORD;
	}
	//-------------------------------------------------------------------------------//
	D3DFILLMODE D3D9Mappings::convertToD3D9(PolygonMode level)
	{
		switch(level)
		{
		case PM_POINTS:
			return D3DFILL_POINT;
		case PM_WIREFRAME:
			return D3DFILL_WIREFRAME;
		case PM_SOLID:
			return D3DFILL_SOLID;
		}
		return D3DFILL_FORCE_DWORD;
	}
	//-------------------------------------------------------------------------------//
	DWORD D3D9Mappings::convertToD3D9(HardwareBuffer::Usage usage)
	{
		DWORD ret = 0;
		if (usage & HardwareBuffer::HBU_DYNAMIC)
		{
			ret |= D3DUSAGE_DYNAMIC;
		}
		if (usage & HardwareBuffer::HBU_WRITE_ONLY)
		{
			ret |= D3DUSAGE_WRITEONLY;
		}
		return ret;
	}
	//-------------------------------------------------------------------------------//
	D3DDECLTYPE D3D9Mappings::convertToD3D9(VertexElementType vType)
	{
		switch (vType)
		{
		case VET_COLOR:
		case VET_COLOR_ABGR:
		case VET_COLOR_ARGB:
			return D3DDECLTYPE_D3DCOLOR;
			break;
		case VET_FLOAT1:
			return D3DDECLTYPE_FLOAT1;
			break;
		case VET_FLOAT2:
			return D3DDECLTYPE_FLOAT2;
			break;
		case VET_FLOAT3:
			return D3DDECLTYPE_FLOAT3;
			break;
		case VET_FLOAT4:
			return D3DDECLTYPE_FLOAT4;
			break;
		case VET_SHORT2:
			return D3DDECLTYPE_SHORT2;
			break;
		case VET_SHORT4:
			return D3DDECLTYPE_SHORT4;
			break;
		case VET_UBYTE4:
			return D3DDECLTYPE_UBYTE4;
			break;
		}
		// to keep compiler happy
		return D3DDECLTYPE_FLOAT3;
	}
	//-------------------------------------------------------------------------------//
	D3DDECLUSAGE D3D9Mappings::convertToD3D9(VertexElementSemantic sem)
	{
		switch (sem)
		{
		case VES_BLEND_INDICES:
			return D3DDECLUSAGE_BLENDINDICES;
			break;
		case VES_BLEND_WEIGHTS:
			return D3DDECLUSAGE_BLENDWEIGHT;
			break;
		case VES_DIFFUSE:
			return D3DDECLUSAGE_COLOR; // NB index will differentiate
			break;
		case VES_SPECULAR:
			return D3DDECLUSAGE_COLOR; // NB index will differentiate
			break;
		case VES_NORMAL:
			return D3DDECLUSAGE_NORMAL;
			break;
		case VES_POSITION:
			return D3DDECLUSAGE_POSITION;
			break;
		case VES_TEXTURE_COORDINATES:
			return D3DDECLUSAGE_TEXCOORD;
			break;
		case VES_BINORMAL:
			return D3DDECLUSAGE_BINORMAL;
			break;
		case VES_TANGENT:
			return D3DDECLUSAGE_TANGENT;
			break;
		}
		// to keep compiler happy
		return D3DDECLUSAGE_POSITION;
	}
	//-------------------------------------------------------------------------------//
	DWORD D3D9Mappings::convertToD3D9(HardwareBuffer::LockOptions options, HardwareBuffer::Usage usage)
	{
		DWORD ret = 0;
		if (options == HardwareBuffer::HBL_DISCARD)
		{
#if OGRE_D3D_MANAGE_BUFFERS
			// Only add the discard flag for dynamic usgae and default pool
			if ((usage & HardwareBuffer::HBU_DYNAMIC) &&
				(usage & HardwareBuffer::HBU_DISCARDABLE))
				ret |= D3DLOCK_DISCARD;
#else
			// D3D doesn't like discard or no_overwrite on non-dynamic buffers
			if (usage & HardwareBuffer::HBU_DYNAMIC)
				ret |= D3DLOCK_DISCARD;
#endif
		}
		if (options == HardwareBuffer::HBL_READ_ONLY)
		{
			// D3D debug runtime doesn't like you locking managed buffers readonly
			// when they were created with write-only (even though you CAN read
			// from the software backed version)
			if (!(usage & HardwareBuffer::HBU_WRITE_ONLY))
				ret |= D3DLOCK_READONLY;

		}
		if (options == HardwareBuffer::HBL_NO_OVERWRITE)
		{
#if OGRE_D3D_MANAGE_BUFFERS
			// Only add the nooverwrite flag for dynamic usgae and default pool
			if ((usage & HardwareBuffer::HBU_DYNAMIC) &&
				(usage & HardwareBuffer::HBU_DISCARDABLE))
				ret |= D3DLOCK_NOOVERWRITE;
#else
			// D3D doesn't like discard or no_overwrite on non-dynamic buffers
			if (usage & HardwareBuffer::HBU_DYNAMIC)
				ret |= D3DLOCK_NOOVERWRITE;
#endif 
		}

		return ret;
	}
	//-------------------------------------------------------------------------------//
	D3DBLEND D3D9Mappings::convertToD3D9(SceneBlendFactor sbf)
	{
		switch( sbf )
		{
		case SBF_ONE:
			return D3DBLEND_ONE;
		case SBF_ZERO:
			return D3DBLEND_ZERO;
		case SBF_DEST_COLOR:
			return D3DBLEND_DESTCOLOR;
		case SBF_SOURCE_COLOR:
			return D3DBLEND_SRCCOLOR;
		case SBF_ONE_MINUS_DEST_COLOR:
			return D3DBLEND_INVDESTCOLOR;
		case SBF_ONE_MINUS_SOURCE_COLOR:
			return D3DBLEND_INVSRCCOLOR;
		case SBF_DEST_ALPHA:
			return D3DBLEND_DESTALPHA;
		case SBF_SOURCE_ALPHA:
			return D3DBLEND_SRCALPHA;
		case SBF_ONE_MINUS_DEST_ALPHA:
			return D3DBLEND_INVDESTALPHA;
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return D3DBLEND_INVSRCALPHA;
		}
		return D3DBLEND_FORCE_DWORD;
	}
	//-------------------------------------------------------------------------------//
	D3DBLENDOP D3D9Mappings::convertToD3D9(SceneBlendOperation sbo)
	{
		switch(sbo)
		{
		case SBO_ADD:
			return D3DBLENDOP_ADD;
		case SBO_SUBTRACT:
			return D3DBLENDOP_SUBTRACT;
		case SBO_REVERSE_SUBTRACT:
			return D3DBLENDOP_REVSUBTRACT;
		case SBO_MIN:
			return D3DBLENDOP_MIN;
		case SBO_MAX:
			return D3DBLENDOP_MAX;
		}

		return D3DBLENDOP_FORCE_DWORD;
	}
	//----------------------------------------------------------------------------//
	D3DTEXTUREADDRESS D3D9Mappings::convertToD3D9(TexAddressMode tam)
	{
		switch(tam)
		{
		case TAM_WARP:
			return D3DTADDRESS_WRAP;
		case TAM_BORDER:
			return D3DTADDRESS_BORDER;
		case TAM_MIRROR:
			return D3DTADDRESS_MIRROR;
		case TAM_CLAMP:
			return D3DTADDRESS_CLAMP;
		default: return D3DTADDRESS_WRAP;
		}
	}
	//----------------------------------------------------------------------------//
	D3DSAMPLERSTATETYPE D3D9Mappings::convertToD3D9(FilterType ft)
	{
		switch (ft)
		{
		case FT_MIN:
			return D3DSAMP_MINFILTER;
			break;
		case FT_MAG:
			return D3DSAMP_MAGFILTER;
			break;
		case FT_MIP:
			return D3DSAMP_MIPFILTER;
			break;
		}

		// to keep compiler happy
		return D3DSAMP_MINFILTER;
	}
	//----------------------------------------------------------------------------//
	DWORD D3D9Mappings::convertToD3D9(FilterType ft, FilterOptions fo)
	{
		switch(ft)
		{
		case FT_MAG:
			{
				switch (fo)
				{
				case FO_POINT:
				case FO_NONE:
				default:
					return D3DTEXF_POINT;
				case FO_LINEAR:
					return D3DTEXF_LINEAR;
				}
			}

		case FT_MIN:
			{
				switch (fo)
				{
				case FO_POINT:
				case FO_NONE:
				default:
					return D3DTEXF_POINT;
				case FO_LINEAR:
					return D3DTEXF_LINEAR;
				}
			}

		case FT_MIP:
			{
				switch (fo)
				{
				case FO_POINT:
					return D3DTEXF_POINT;
				case FO_NONE:
				default:
					return D3DTEXF_NONE;
				case FO_LINEAR:
					return D3DTEXF_LINEAR;
				}
			}
		}
		return 0;
	}
	//----------------------------------------------------------------------------//
	DWORD D3D9Mappings::convertToD3D9(CompareFunction cf)
	{
		switch( cf )
		{
		case CMPF_ALWAYS_FAIL:
			return D3DCMP_NEVER;
		case CMPF_ALWAYS_PASS:
			return D3DCMP_ALWAYS;
		case CMPF_LESS:
			return D3DCMP_LESS;
		case CMPF_LESS_EQUAL:
			return D3DCMP_LESSEQUAL;
		case CMPF_EQUAL:
			return D3DCMP_EQUAL;
		case CMPF_NOT_EQUAL:
			return D3DCMP_NOTEQUAL;
		case CMPF_GREATER_EQUAL:
			return D3DCMP_GREATEREQUAL;
		case CMPF_GREATER:
			return D3DCMP_GREATER;
		};
		return 0;
	}
	//-------------------------------------------------------------------------------//
	D3DXMATRIX D3D9Mappings::makeD3DXMatrix(const Matrix4& mat, bool isColumn)
	{
		// Transpose matrix
		// D3D9 uses row vectors i.e. V*M
		// Ogre, OpenGL and everything else uses column vectors i.e. M*V
		if(!isColumn)
			return D3DXMATRIX(
				mat[0][0], mat[1][0], mat[2][0], mat[3][0],
				mat[0][1], mat[1][1], mat[2][1], mat[3][1],
				mat[0][2], mat[1][2], mat[2][2], mat[3][2],
				mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
		else
			return D3DXMATRIX(
			mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
	}
	//-------------------------------------------------------------------------------//
	Matrix4 D3D9Mappings::convertD3DXMatrix( const D3DXMATRIX& mat )
	{
		return Matrix4(
			mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
			mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
			mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
			mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
	}
	//-------------------------------------------------------------------------------//
	D3DFORMAT D3D9Mappings::convertD3D9Format(PixelFormat format)
	{
		switch (format)
		{
		case PF_L8:
			return D3DFMT_L8;
		case PF_L16:
			return D3DFMT_L16;
		case PF_A8:
			return D3DFMT_A8;
		case PF_A4L4:
			return D3DFMT_A4L4;
		case PF_BYTE_LA:
			return D3DFMT_A8L8; // Assume little endian here
		case PF_R3G3B2:
			return D3DFMT_R3G3B2;
		case PF_A1R5G5B5:
			return D3DFMT_A1R5G5B5;
		case PF_R5G6B5:
			return D3DFMT_R5G6B5;
		case PF_A4R4G4B4:
			return D3DFMT_A4R4G4B4;
		case PF_R8G8B8:
			return D3DFMT_R8G8B8;
		case PF_A8R8G8B8:
			return D3DFMT_A8R8G8B8;
		case PF_A8B8G8R8:
			return D3DFMT_A8B8G8R8;
		case PF_X8R8G8B8:
			return D3DFMT_X8R8G8B8;
		case PF_X8B8G8R8:
			return D3DFMT_X8B8G8R8;
		case PF_A2B10G10R10:
			return D3DFMT_A2B10G10R10;
		case PF_A2R10G10B10:
			return D3DFMT_A2R10G10B10;
		case PF_FLOAT16_R:
			return D3DFMT_R16F;
		case PF_FLOAT16_GR:
			return D3DFMT_G16R16F;
		case PF_FLOAT16_RGBA:
			return D3DFMT_A16B16G16R16F;
		case PF_FLOAT32_R:
			return D3DFMT_R32F;
		case PF_FLOAT32_GR:
			return D3DFMT_G32R32F;
		case PF_FLOAT32_RGBA:
			return D3DFMT_A32B32G32R32F;
		case PF_SHORT_RGBA:
			return D3DFMT_A16B16G16R16;
		case PF_SHORT_GR:
			return D3DFMT_G16R16;
		case PF_DXT1:
			return D3DFMT_DXT1;
		case PF_DXT2:
			return D3DFMT_DXT2;
		case PF_DXT3:
			return D3DFMT_DXT3;
		case PF_DXT4:
			return D3DFMT_DXT4;
		case PF_DXT5:
			return D3DFMT_DXT5;
		case PF_UNKNOWN:
		default:
			return D3DFMT_UNKNOWN;
		}
	}
	//-------------------------------------------------------------------------------//
	PixelFormat D3D9Mappings::convertToTitan(D3DFORMAT format)
	{
		switch(format)
		{
		case D3DFMT_A8:
			return PF_A8;
		case D3DFMT_L8:
			return PF_L8;
		case D3DFMT_L16:
			return PF_L16;
		case D3DFMT_A4L4:
			return PF_A4L4;
		case D3DFMT_A8L8:
			return PF_BYTE_LA;	// Assume little endian here
		case D3DFMT_R3G3B2:
			return PF_R3G3B2;
		case D3DFMT_A1R5G5B5:
			return PF_A1R5G5B5;
		case D3DFMT_A4R4G4B4:
			return PF_A4R4G4B4;
		case D3DFMT_R5G6B5:
			return PF_R5G6B5;
		case D3DFMT_R8G8B8:
			return PF_R8G8B8;
		case D3DFMT_X8R8G8B8:
			return PF_X8R8G8B8;
		case D3DFMT_A8R8G8B8:
			return PF_A8R8G8B8;
		case D3DFMT_X8B8G8R8:
			return PF_X8B8G8R8;
		case D3DFMT_A8B8G8R8:
			return PF_A8B8G8R8;
		case D3DFMT_A2R10G10B10:
			return PF_A2R10G10B10;
		case D3DFMT_A2B10G10R10:
			return PF_A2B10G10R10;
		case D3DFMT_R16F:
			return PF_FLOAT16_R;
		case D3DFMT_A16B16G16R16F:
			return PF_FLOAT16_RGBA;
		case D3DFMT_R32F:
			return PF_FLOAT32_R;
		case D3DFMT_G32R32F:
			return PF_FLOAT32_GR;
		case D3DFMT_A32B32G32R32F:
			return PF_FLOAT32_RGBA;
		case D3DFMT_G16R16F:
			return PF_FLOAT16_GR;
		case D3DFMT_A16B16G16R16:
			return PF_SHORT_RGBA;
		case D3DFMT_G16R16:
			return PF_SHORT_GR;
		case D3DFMT_DXT1:
			return PF_DXT1;
		case D3DFMT_DXT2:
			return PF_DXT2;
		case D3DFMT_DXT3:
			return PF_DXT3;
		case D3DFMT_DXT4:
			return PF_DXT4;
		case D3DFMT_DXT5:
			return PF_DXT5;
		default:
			return PF_UNKNOWN;
		}
	}
	//-------------------------------------------------------------------------------//
	D3DPOOL D3D9Mappings::convertD3D9Pool(TexPool pool)
	{
		switch (pool)
		{
		case TP_MANAGED:
			return D3DPOOL_MANAGED;
		case TP_SYSMEM:
			return D3DPOOL_SYSTEMMEM;
		case TP_DEFAULT:
		default:
			return D3DPOOL_DEFAULT;
		}
	}
	//-------------------------------------------------------------------------------//
	ShaderConstantType D3D9Mappings::convertConstantType(D3DXPARAMETER_TYPE type)
	{
		switch(type)
		{
		case D3DXPT_INT: 
			return SCT_INT1;

		case D3DXPT_FLOAT:
			return SCT_FLOAT1;

		case D3DXPT_SAMPLER1D:
			return SCT_SAMPLER1D;

		case D3DXPT_SAMPLER2D:
			return SCT_SAMPLER2D;

		case D3DXPT_SAMPLER3D:
			return SCT_SAMPLER3D;

		case D3DXPT_SAMPLERCUBE:
			return SCT_SAMPLERCUBE;

			//add other in the future
		default:
			return SCT_UNUSED;
		}
	}
	//------------------------------------------------------------------------------//
	D3DXIMAGE_FILEFORMAT D3D9Mappings::convertToD3D9(const String& extName)
	{
		if(extName == "bmp")
			return D3DXIFF_BMP;
		else if(extName == "jpg")
			return D3DXIFF_JPG;
		else if(extName == "tga")
			return D3DXIFF_TGA;
		else if(extName == "png")
			return D3DXIFF_PNG;
		else if(extName == "dds")
			return D3DXIFF_DDS;
		else
			return D3DXIFF_FORCE_DWORD;
	}

}