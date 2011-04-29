#ifndef __TITAN_DTAT_TYPE__HH
#define __TITAN_DTAT_TYPE__HH

namespace Titan
{
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	typedef int int32;
	typedef short int16;
	typedef char int8;

	typedef __int64   int64;
	typedef unsigned __int64  uint64;

	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

	extern const int32	MIN_INT32;
	extern const int32	MAX_INT32;
	extern const int16	MIN_INT16;
	extern const int16	MAX_INT16;
	extern const int8	MIN_INT8;
	extern const int8	MAX_INT8;
	extern const uint32	MIN_UINT32;
	extern const uint32	MAX_UINT32;
	extern const uint16	MIN_UINT16;
	extern const uint16	MAX_UINT16;
	extern const uint8	MIN_UINT8;
	extern const uint8	MAX_UINT8;

	extern const float	MIN_REAL32;
	extern const float	MAX_REAL32;
	extern const float	TINY_REAL32;
}
#endif