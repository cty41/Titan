#include "TitanStableHeader.h"
#include "TiDataType.h"

namespace Titan
{
	const int32	    Titan::MIN_INT32   =   (int32)0xffffffff; // (-2147483646);
	const int32	    Titan::MAX_INT32   =   (int32)0x7fffffff; // ( 2147483647);
	const int16	    Titan::MIN_INT16   =   (int16)0xffff; // (-32768);
	const int16	    Titan::MAX_INT16   =   (int16)0x7fff; // ( 32767);
	const int8	    Titan::MIN_INT8    =   (int8)0xff; // (-128);
	const int8	    Titan::MAX_INT8    =   (int8)0x7f; // ( 127);
	const uint32    Titan::MIN_UINT32  =   (uint32)0;
	const uint32    Titan::MAX_UINT32  =   (uint32)0xffffffff;
	const uint16    Titan::MIN_UINT16  =   (uint16)0;
	const uint16    Titan::MAX_UINT16  =   (uint16)0xffff;
	const uint8	    Titan::MIN_UINT8   =   (uint8)0;
	const uint8	    Titan::MAX_UINT8   =   (uint8)0xff;

	const float	Titan::MAX_REAL32  =   static_cast<float>(3.4E+38);
	const float	Titan::MIN_REAL32  =   static_cast<float>(-3.4E+38);
	const float	Titan::TINY_REAL32  =   static_cast<float>(3.4E-38);
}