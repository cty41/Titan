#include "D3D9Mappings.h"
#include "TitanMatrix4.h"

namespace Titan
{
	//---------------------------------------------------------------------
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
	//---------------------------------------------------------------------
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
		case CULL_ANTICLOCKWISE:
			if( flip )
				return D3DCULL_CW;
			else
				return D3DCULL_CCW;
		}
		return 0;
	}
	//---------------------------------------------------------------------
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
	//---------------------------------------------------------------------
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//---------------------------------------------------------------------
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	D3DXMATRIX D3D9Mappings::makeD3DXMatrix( const Matrix4& mat )
	{
		// Transpose matrix
		// D3D9 uses row vectors i.e. V*M
		// Ogre, OpenGL and everything else uses column vectors i.e. M*V
		return D3DXMATRIX(
			mat[0][0], mat[1][0], mat[2][0], mat[3][0],
			mat[0][1], mat[1][1], mat[2][1], mat[3][1],
			mat[0][2], mat[1][2], mat[2][2], mat[3][2],
			mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
	}
	//---------------------------------------------------------------------
	Matrix4 D3D9Mappings::convertD3DXMatrix( const D3DXMATRIX& mat )
	{
		return Matrix4(
			mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
			mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
			mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
			mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
	}
}