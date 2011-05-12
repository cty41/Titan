#ifndef __TITAN_GEOTERRAIN_DEFS_HH
#define __TITAN_GEOTERRAIN_DEFS_HH

#include "TiPrerequisites.h"

namespace Titan
{
	struct GeoIndexData
	{
		ushort	flag;
		ushort*	index_table;
		ushort	vertex_count;
	};

#define STRIP_LIST 0xffff
#define	TRIANGLE_LIST 0xfffe

	static unsigned short base_level_0_tile[] = 
{
	STRIP_LIST, 0,72,8,80
};

//
// detail level 1
//

// tile 0	(0000h)
static unsigned short base_level_1_tile0[] = 
{
	STRIP_LIST, 0, 36, 4, 40, 8, 44, 44, 44, 80, 40, 76, 36, 72
};

// tile 1	(0001h)
static unsigned short base_level_1_tile1[] = 
{
	TRIANGLE_LIST, 
	36,40,0,	//Left
	72,40,36,	
	40,44,8,	//Right
	40,80,44,	
	72,76,40,	//Bottom
	76,80,40,	
};

// tile 2	(0010h)
static unsigned short base_level_1_tile2[] = 
{
	TRIANGLE_LIST, 
	40,4,0,		//Top
	40,8,4,		
	40,44,8,	//Right
	40,80,44,	
	72,76,40,	//Bottom
	76,80,40,	
};
// tile 3	(0011h)
static unsigned short base_level_1_tile3[] = 
{
	TRIANGLE_LIST, 
	40,44,8,	//Right
	40,80,44,	
	72,76,40,	//Bottom
	76,80,40,	
};

// tile 4	(0100h)
static unsigned short base_level_1_tile4[] = 
{
	TRIANGLE_LIST, 
	40,4,0,		//Top
	40,8,4,		
	36,40,0,	//Left
	72,40,36,	
	72,76,40,	//Bottom
	76,80,40,	
};
// tile 5	(0101h)
static unsigned short base_level_1_tile5[] = 
{
	TRIANGLE_LIST, 
	36,40,0,	//Left
	72,40,36,	
	72,76,40,	//Bottom
	76,80,40,	
};
// tile 6	(0110h)
static unsigned short base_level_1_tile6[] = 
{
	TRIANGLE_LIST, 
	40,4,0,		//Top
	40,8,4,		
	72,76,40,	//Bottom
	76,80,40,	
};
// tile 7	(0111h)
static unsigned short base_level_1_tile7[] = 
{
	STRIP_LIST, 
	72,76,40,80	//Bottom
};

// tile 8	(1000h)
static unsigned short base_level_1_tile8[] = 
{
	TRIANGLE_LIST, 
	40,4,0,		//Top
	40,8,4,		
	36,40,0,	//Left
	72,40,36,	
	40,44,8,	//Right
	40,80,44,	
};
// tile 9	(1001h)
static unsigned short base_level_1_tile9[] = 
{
	TRIANGLE_LIST, 
	36,40,0,	//Left
	72,40,36,	
	40,44,8,	//Right
	40,80,44,	
};
// tile 10	(1010h)
static unsigned short base_level_1_tile10[] = 
{
	TRIANGLE_LIST, 
	40,4,0,		//Top
	40,8,4,		
	40,44,8,	//Right
	40,80,44,	
};
// tile 11	(1011h)
static unsigned short base_level_1_tile11[] = 
{
	STRIP_LIST, 
	8,40,44,80	//Right
};

// tile 12	(1100h)
static unsigned short base_level_1_tile12[] = 
{
	TRIANGLE_LIST, 
	40,4,0,		//Top
	40,8,4,		
	36,40,0,	//Left
	72,40,36,	
};
// tile 13	(1101h)
static unsigned short base_level_1_tile13[] = 
{
	STRIP_LIST, 
	0, 36,40,72,	//Left
};
// tile 14	(1110h)
static unsigned short base_level_1_tile14[] = 
{
	STRIP_LIST, 
	0,40,4,8	//Top
};
// tile 15	(1111h)
static unsigned short base_level_1_tile15[] = 
{
	// no tile - pure connectors ( we handle this case in the cSubMesh:render() routine)
	TRIANGLE_LIST, 0, 0, 0
};


//
// detail level 2
//

// tile 0	(0000h)
static unsigned short base_level_2_tile0[] = 
{
	STRIP_LIST, 
	0, 18, 2, 20, 4, 22, 6, 24, 8,	26, 26,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	54, 54, 72, 56, 74, 58, 76, 60, 78, 62, 80
};
// tile 1	(0001h)
static unsigned short base_level_2_tile1[] = 
{
	STRIP_LIST, 
	0, 18, 20, 20, 24, 24, 8, 26, 26,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	54, 54, 72, 56, 74, 58, 76, 60, 78, 62, 80
};
// tile 2	(0010h)
static unsigned short base_level_2_tile2[] = 
{
	STRIP_LIST, 
	0, 20, 2, 20, 2, 20, 4, 22, 6, 24, 8, 26, 26,
	20, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	38, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	72, 72, 72, 56, 74, 58, 76, 60, 78, 62, 80
};
// tile 3	(0011h)
static unsigned short base_level_2_tile3[] = 
{
	STRIP_LIST, 
	24, 24, 8, 26 ,26,
	20, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	38, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	72, 72, 72, 56, 74, 58, 76, 60, 78, 62, 80
};

// tile 4	(0100h)
static unsigned short base_level_2_tile4[] = 
{
	STRIP_LIST, 
	0, 18, 2, 20, 4, 22, 6, 24, 8, 24, 24,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 42,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 60,
	54, 54, 72, 56, 74, 58, 76, 60, 78, 80
};
// tile 5	(0101h)
static unsigned short base_level_2_tile5[] = 
{
	STRIP_LIST, 
	0, 18, 20, 20, 20,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 42,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 60,
	54, 54, 72, 56, 74, 58, 76, 60, 78, 80
};
// tile 6	(0110h)
static unsigned short base_level_2_tile6[] = 
{
	STRIP_LIST, 
	0, 20, 2, 20, 2, 20, 4, 22, 6, 24, 8, 24, 24,
	20, 20, 38, 22, 40, 24, 42, 42,
	38, 38, 56, 40, 58, 42, 60, 60,
	72, 72, 72, 56, 74, 58, 76, 60, 78, 80
};
// tile 7	(0111h)
static unsigned short base_level_2_tile7[] = 
{
	STRIP_LIST, 
	20, 38, 22, 40, 24, 42, 42,
	38, 38, 56, 40, 58, 42, 60, 60,
	72, 72, 72, 56, 74, 58, 76, 60, 78, 80
};

// tile 8	(1000h)
static unsigned short base_level_2_tile8[] = 
{
	STRIP_LIST, 
	0, 18, 2, 20, 4, 22, 6, 24, 8,	26, 26,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	54, 54, 72, 56, 56, 60, 60, 62, 80
};
// tile 9	(1001h)
static unsigned short base_level_2_tile9[] = 
{
	STRIP_LIST, 
	0, 18, 20, 20, 24, 24, 8, 26, 26,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	54, 54, 72, 56, 56, 60, 60, 62, 80
};
// tile 10	(1010h)
static unsigned short base_level_2_tile10[] = 
{
	STRIP_LIST, 
	0, 20, 2, 20, 2, 20, 4, 22, 6, 24, 8, 26, 26,
	20, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	38, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	62, 62, 60, 80
};
// tile 11	(1011h)
static unsigned short base_level_2_tile11[] = 
{
	STRIP_LIST, 
	24, 24, 8, 26 ,26,
	20, 20, 38, 22, 40, 24, 42, 26, 44, 44,
	38, 38, 56, 40, 58, 42, 60, 44, 62, 62,
	62, 62, 60, 80
};

// tile 12	(1100h)
static unsigned short base_level_2_tile12[] = 
{
	STRIP_LIST, 
	0, 18, 2, 20, 4, 22, 6, 24, 8, 24, 24,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 42,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 60,
	54, 54, 72, 56
};
// tile 13	(1101h)
static unsigned short base_level_2_tile13[] = 
{
	STRIP_LIST, 
	0, 18, 20, 20, 20,
	18, 18, 36, 20, 38, 22, 40, 24, 42, 42,
	36, 36, 54, 38, 56, 40, 58, 42, 60, 60,
	54, 54, 72, 56
};
// tile 14	(1110h)
static unsigned short base_level_2_tile14[] = 
{
	STRIP_LIST, 
	0, 20, 2, 20, 2, 20, 4, 22, 6, 24, 8, 24, 24,
	20, 20, 38, 22, 40, 24, 42, 42,
	38, 38, 56, 40, 58, 42, 60
};
// tile 15	(1111h)
static unsigned short base_level_2_tile15[] = 
{
	STRIP_LIST, 
	20, 38, 22, 40, 24, 42, 42,
	38, 38, 56, 40, 58, 42, 60, 60,
};


//
// detail level 3
//

// tile 0	(0000h)
static unsigned short base_level_3_tile0[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 17, 17,
	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	63, 63, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 71, 80
};
// tile 1	(0001h)
static unsigned short base_level_3_tile1[] = 
{
	STRIP_LIST, 
	0, 9, 10, 10, 16, 16, 8, 17, 17,	// draw corners of the top line...
	9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, // prime the cache with degenerate triangles

	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	63, 63, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 71, 80
};
// tile 2	(0010h)
static unsigned short base_level_3_tile2[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 10, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 17, 17,
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	72, 72, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 71, 80
};
// tile 3	(0011h)
static unsigned short base_level_3_tile3[] = 
{
	STRIP_LIST, 
	8, 16, 17, 17, 17,	// draw corner of the top line...
	10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,  // prime the cache with degenerate triangles
	
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	72, 72, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 71, 80
};

// tile 4	(0000h)
static unsigned short base_level_3_tile4[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 8, 8,
	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
	63, 63, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 80
};
// tile 5	(0001h)
static unsigned short base_level_3_tile5[] = 
{
	STRIP_LIST, 
	0, 9, 10, // draw corners of the top line...
	10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, // prime the cache with degenerate triangles
	
	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
	63, 63, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 80
};
// tile 6	(0010h)
static unsigned short base_level_3_tile6[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 10, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 8, 8,
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
	72, 72, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 80
};
// tile 7	(0011h)
static unsigned short base_level_3_tile7[] = 
{
	STRIP_LIST, 
	9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,// prime the cache with degenerate triangles
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
	72, 72, 72, 64, 73, 65, 74, 66, 75, 67, 76, 68, 77, 69, 78, 70, 79, 80
};

// tile 8	(1000h)
static unsigned short base_level_3_tile8[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 17, 17,
	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	63, 63, 72, 64, 64, 70, 70, 71, 80
};
// tile 9	(1001h)
static unsigned short base_level_3_tile9[] = 
{
	STRIP_LIST, 
	0, 9, 10, 10, 16, 16, 8, 17, 17,	// draw corners of the top line...
	9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, // prime the cache with degenerate triangles

	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	63, 63, 72, 64, 64, 70, 70, 71, 80
};
// tile 10	(1010h)
static unsigned short base_level_3_tile10[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 10, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 17, 17,
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	71, 71, 70, 80
};
// tile 11	(1011h)
static unsigned short base_level_3_tile11[] = 
{
	STRIP_LIST, 
	8, 16, 17, 17, 17,	// draw corner of the top line...
	10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, // prime the cache with degenerate triangles
	
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 17, 26, 26,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 26, 35, 35,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 35, 44, 44,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 44, 53, 53,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 53, 62, 62,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 62, 71, 71,
	71, 71, 70, 80
};

// tile 12	(1100h)
static unsigned short base_level_3_tile12[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 8, 8,
	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
	63, 63, 72, 64
};
// tile 13	(1101h)
static unsigned short base_level_3_tile13[] = 
{
	STRIP_LIST, 
	0, 9, 10, 10, // draw corners of the top line...
	10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, // prime the cache with degenerate triangles
	
	9, 9, 18, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	18, 18, 27, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	27, 27, 36, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	36, 36, 45, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	45, 45, 54, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	54, 54, 63, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
	63, 63, 72, 64
};
// tile 14	(1110h)
static unsigned short base_level_3_tile14[] = 
{
	STRIP_LIST, 
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,0, // prime the cache with degenerate triangles

	0, 10, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14 ,6, 15, 7, 16, 8, 8, 8,
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
};
// tile 15	(1111h)
static unsigned short base_level_3_tile15[] = 
{
	STRIP_LIST, 
	9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,// prime the cache with degenerate triangles
	10, 10, 19, 11, 20, 12, 21, 13, 22, 14, 23, 15, 24, 16, 25, 25,
	19, 19, 28, 20, 29, 21, 30, 22, 31, 23, 32, 24, 33, 25, 34, 34,
	28, 28, 37, 29, 38, 30, 39, 31, 40, 32, 41, 33, 42, 34, 43, 43,
	37, 37, 46, 38, 47, 39, 48, 40, 49, 41, 50, 42, 51, 43, 52, 52,
	46, 46, 55, 47, 56, 48, 57, 49, 58, 50, 59, 51, 60, 52, 61, 61,
	55, 55, 64, 56, 65, 57, 66, 58, 67, 59, 68, 60, 69, 61, 70, 70,
};

//
// Tile Connectors
//

static unsigned short Connect1to0[4][4]=
{
	//Top
	{
		TRIANGLE_LIST, 
		0,40,8
	},
	//Left
	{
		TRIANGLE_LIST, 
		72,40,0
	},
	//Right
	{
		TRIANGLE_LIST, 
		40,80,8
	},
	//Bottom
	{
		TRIANGLE_LIST, 
		72,80,40
	}
};

//
// DETAIL LEVEL 2
//

static unsigned short Connect2to1[4][7]=
{
	//Top
	{
		STRIP_LIST, 
		0, 20, 4, 22, 8, 24
	},
	//Left
	{
		STRIP_LIST, 
		72,56,36,38,0,20
	},
	//Right
	{
		STRIP_LIST, 
		8,24,44,42,80,60
	},
	//Bottom
	{
		STRIP_LIST, 
		80,60,76,58,72,56
	}
};


static unsigned short Connect2to0[4][6]=
{
	//Top
	{
		STRIP_LIST, 
		0,20,8,22,24
	},
	//Left
	{
		STRIP_LIST, 
		72,56,0,38,20
	},
	//Right
	{
		STRIP_LIST, 
		8,24,80,42,60
	},
	//Bottom
	{
		STRIP_LIST, 
		80,60,72,58,56
	}
};


//
// DETAIL LEVEL 3
//

static unsigned short Connect3to2[4][15]=
{
	//Top
	{
		STRIP_LIST, 
		0,10,2,11,4,12,4,13,6,14,6,15,8,16
	},
	//Left
	{
		STRIP_LIST, 
		72,64,54,55,36,46,36,37,18,28,18,19,0,10
	},
	//Right
	{
		STRIP_LIST, 
		8,16,26,25,44,34,44,43,62,52,62,61,80,70
	},
	//Bottom
	{
		STRIP_LIST, 
		80,70,78,69,76,68,76,67,74,66,74,65,72,64
	}
};

/*
static unsigned short Connect3to2[4][31]=
{
	//Top
	{
		TRIANGLE_LIST, 
		10,11,0,
		11,2,0,
		11,12,2,
		12,13,2,
		13,4,2,
		13,6,4,
		13,14,6,
		14,15,6,
		15,8,6,
		15,16,8
	},
	//Left
	{
		TRIANGLE_LIST, 
		19,10,0,
		19,0,18,
		28,19,18,
		37,28,18,
		37,18,36,
		36,54,37,
		54,46,37,
		54,55,46,
		54,72,55,
		72,64,55
	},
	//Right
	{
		TRIANGLE_LIST, 
		8,16,25,
		8,25,26,
		26,25,34,
		26,34,43,
		26,43,44,
		62,44,43,
		62,43,52,
		62,52,61,
		80,62,61,
		80,61,70,
	},
	//Bottom
	{
		TRIANGLE_LIST, 
		72,65,64,
		72,74,65,
		74,66,65,
		74,67,66,
		74,76,67,
		78,67,76,
		78,68,67,
		78,69,68,
		80,69,78,
		80,70,69
	}
};
*/

static unsigned short Connect3to1[4][25]=
{
	//Top
	{
		TRIANGLE_LIST, 
		0,10,11,
		0,11,12,
		0,12,13,
		0,13,4,
		8,4,13,
		8,13,14,
		8,14,15,
		8,15,16
	},
	//Left
	{
		TRIANGLE_LIST, 
		0,36,37,
		0,37,28,
		0,28,19,
		0,19,10,
		72,37,36,
		72,46,37,
		72,55,46,
		72,64,55
	},
	//Right
	{
		TRIANGLE_LIST, 
		8,43,44,
		8,34,43,
		8,25,34,
		8,16,25,
		80,44,43,
		80,43,52,
		80,52,61,
		80,61,70
	},
	//Bottom
	{
		TRIANGLE_LIST, 
		72,76,67,
		72,67,66,
		72,66,65,
		72,65,64,
		80,67,76,
		80,68,67,
		80,69,68,
		80,70,69
	}
};

static unsigned short Connect3to0[4][22]=
{
	//Top
	{
		TRIANGLE_LIST, 
		0,10,11,
		0,11,12,
		0,12,13,
		0,13,8,
		8,13,14,
		8,14,15,
		8,15,16
	},
	//Left
	{
		TRIANGLE_LIST, 
		0,37,28,
		0,28,19,
		0,19,10,
		72,37,0,
		72,46,37,
		72,55,46,
		72,64,55
	},
	//Right
	{
		TRIANGLE_LIST, 
		8,43,80,
		8,34,43,
		8,25,34,
		8,16,25,
		80,43,52,
		80,52,61,
		80,61,70
	},
	//Bottom
	{
		TRIANGLE_LIST, 
		72,80,67,
		72,67,66,
		72,66,65,
		72,65,64,
		80,68,67,
		80,69,68,
		80,70,69
	}


};//

static GeoIndexData base_level_0 = 
{
	base_level_0_tile[0], &base_level_0_tile[1], (sizeof(base_level_0_tile)/sizeof(short))-1
};


static GeoIndexData base_level_1[16] = 
{
	{base_level_1_tile0[0], &base_level_1_tile0[1], (sizeof(base_level_1_tile0)/sizeof(short))-1},	
	{base_level_1_tile1[0], &base_level_1_tile1[1], (sizeof(base_level_1_tile1)/sizeof(short))-1},	
	{base_level_1_tile2[0], &base_level_1_tile2[1], (sizeof(base_level_1_tile2)/sizeof(short))-1},	
	{base_level_1_tile3[0], &base_level_1_tile3[1], (sizeof(base_level_1_tile3)/sizeof(short))-1},	
	{base_level_1_tile4[0], &base_level_1_tile4[1], (sizeof(base_level_1_tile4)/sizeof(short))-1},	
	{base_level_1_tile5[0], &base_level_1_tile5[1], (sizeof(base_level_1_tile5)/sizeof(short))-1},	
	{base_level_1_tile6[0], &base_level_1_tile6[1], (sizeof(base_level_1_tile6)/sizeof(short))-1},	
	{base_level_1_tile7[0], &base_level_1_tile7[1], (sizeof(base_level_1_tile7)/sizeof(short))-1},	
	{base_level_1_tile8[0], &base_level_1_tile8[1], (sizeof(base_level_1_tile8)/sizeof(short))-1},	
	{base_level_1_tile9[0], &base_level_1_tile9[1], (sizeof(base_level_1_tile9)/sizeof(short))-1},	
	{base_level_1_tile10[0], &base_level_1_tile10[1], (sizeof(base_level_1_tile10)/sizeof(short))-1},	
	{base_level_1_tile11[0], &base_level_1_tile11[1], (sizeof(base_level_1_tile11)/sizeof(short))-1},	
	{base_level_1_tile12[0], &base_level_1_tile12[1], (sizeof(base_level_1_tile12)/sizeof(short))-1},	
	{base_level_1_tile13[0], &base_level_1_tile13[1], (sizeof(base_level_1_tile13)/sizeof(short))-1},	
	{base_level_1_tile14[0], &base_level_1_tile14[1], (sizeof(base_level_1_tile14)/sizeof(short))-1},	
	{base_level_1_tile15[0], &base_level_1_tile15[1], (sizeof(base_level_1_tile15)/sizeof(short))-1},	
};

static GeoIndexData base_level_2[16] = 
{
	{base_level_2_tile0[0], &base_level_2_tile0[1], (sizeof(base_level_2_tile0)/sizeof(short))-1},	
	{base_level_2_tile1[0], &base_level_2_tile1[1], (sizeof(base_level_2_tile1)/sizeof(short))-1},	
	{base_level_2_tile2[0], &base_level_2_tile2[1], (sizeof(base_level_2_tile2)/sizeof(short))-1},	
	{base_level_2_tile3[0], &base_level_2_tile3[1], (sizeof(base_level_2_tile3)/sizeof(short))-1},	
	{base_level_2_tile4[0], &base_level_2_tile4[1], (sizeof(base_level_2_tile4)/sizeof(short))-1},	
	{base_level_2_tile5[0], &base_level_2_tile5[1], (sizeof(base_level_2_tile5)/sizeof(short))-1},	
	{base_level_2_tile6[0], &base_level_2_tile6[1], (sizeof(base_level_2_tile6)/sizeof(short))-1},	
	{base_level_2_tile7[0], &base_level_2_tile7[1], (sizeof(base_level_2_tile7)/sizeof(short))-1},	
	{base_level_2_tile8[0], &base_level_2_tile8[1], (sizeof(base_level_2_tile8)/sizeof(short))-1},	
	{base_level_2_tile9[0], &base_level_2_tile9[1], (sizeof(base_level_2_tile9)/sizeof(short))-1},	
	{base_level_2_tile10[0], &base_level_2_tile10[1], (sizeof(base_level_2_tile10)/sizeof(short))-1},	
	{base_level_2_tile11[0], &base_level_2_tile11[1], (sizeof(base_level_2_tile11)/sizeof(short))-1},	
	{base_level_2_tile12[0], &base_level_2_tile12[1], (sizeof(base_level_2_tile12)/sizeof(short))-1},	
	{base_level_2_tile13[0], &base_level_2_tile13[1], (sizeof(base_level_2_tile13)/sizeof(short))-1},	
	{base_level_2_tile14[0], &base_level_2_tile14[1], (sizeof(base_level_2_tile14)/sizeof(short))-1},	
	{base_level_2_tile15[0], &base_level_2_tile15[1], (sizeof(base_level_2_tile15)/sizeof(short))-1},	
};

static GeoIndexData base_level_3[16] = 
{
	{base_level_3_tile0[0], &base_level_3_tile0[1], (sizeof(base_level_3_tile0)/sizeof(short))-1},	
	{base_level_3_tile1[0], &base_level_3_tile1[1], (sizeof(base_level_3_tile1)/sizeof(short))-1},	
	{base_level_3_tile2[0], &base_level_3_tile2[1], (sizeof(base_level_3_tile2)/sizeof(short))-1},	
	{base_level_3_tile3[0], &base_level_3_tile3[1], (sizeof(base_level_3_tile3)/sizeof(short))-1},	
	{base_level_3_tile4[0], &base_level_3_tile4[1], (sizeof(base_level_3_tile4)/sizeof(short))-1},	
	{base_level_3_tile5[0], &base_level_3_tile5[1], (sizeof(base_level_3_tile5)/sizeof(short))-1},	
	{base_level_3_tile6[0], &base_level_3_tile6[1], (sizeof(base_level_3_tile6)/sizeof(short))-1},	
	{base_level_3_tile7[0], &base_level_3_tile7[1], (sizeof(base_level_3_tile7)/sizeof(short))-1},	
	{base_level_3_tile8[0], &base_level_3_tile8[1], (sizeof(base_level_3_tile8)/sizeof(short))-1},	
	{base_level_3_tile9[0], &base_level_3_tile9[1], (sizeof(base_level_3_tile9)/sizeof(short))-1},	
	{base_level_3_tile10[0], &base_level_3_tile10[1], (sizeof(base_level_3_tile10)/sizeof(short))-1},	
	{base_level_3_tile11[0], &base_level_3_tile11[1], (sizeof(base_level_3_tile11)/sizeof(short))-1},	
	{base_level_3_tile12[0], &base_level_3_tile12[1], (sizeof(base_level_3_tile12)/sizeof(short))-1},	
	{base_level_3_tile13[0], &base_level_3_tile13[1], (sizeof(base_level_3_tile13)/sizeof(short))-1},	
	{base_level_3_tile14[0], &base_level_3_tile14[1], (sizeof(base_level_3_tile14)/sizeof(short))-1},	
	{base_level_3_tile15[0], &base_level_3_tile15[1], (sizeof(base_level_3_tile15)/sizeof(short))-1},	
};

static GeoIndexData connect_level_1_to_0[4]= 
{
	{Connect1to0[0][0], &Connect1to0[0][1], 3},	
	{Connect1to0[1][0], &Connect1to0[1][1], 3},	
	{Connect1to0[2][0], &Connect1to0[2][1], 3},	
	{Connect1to0[3][0], &Connect1to0[3][1], 3},	
};

static GeoIndexData connect_level_2_to_1[4]= 
{
	{Connect2to1[0][0], &Connect2to1[0][1], 6},	
	{Connect2to1[1][0], &Connect2to1[1][1], 6},	
	{Connect2to1[2][0], &Connect2to1[2][1], 6},	
	{Connect2to1[3][0], &Connect2to1[3][1], 6},	
};

static GeoIndexData connect_level_2_to_0[4]= 
{
	{Connect2to0[0][0], &Connect2to0[0][1], 5},	
	{Connect2to0[1][0], &Connect2to0[1][1], 5},	
	{Connect2to0[2][0], &Connect2to0[2][1], 5},	
	{Connect2to0[3][0], &Connect2to0[3][1], 5},	
};

static GeoIndexData connect_level_3_to_2[4]= 
{
	{Connect3to2[0][0], &Connect3to2[0][1], 14},	
	{Connect3to2[1][0], &Connect3to2[1][1], 14},	
	{Connect3to2[2][0], &Connect3to2[2][1], 14},	
	{Connect3to2[3][0], &Connect3to2[3][1], 14},	
};

static GeoIndexData connect_level_3_to_1[4]= 
{
	{Connect3to1[0][0], &Connect3to1[0][1], 24},	
	{Connect3to1[1][0], &Connect3to1[1][1], 24},	
	{Connect3to1[2][0], &Connect3to1[2][1], 24},	
	{Connect3to1[3][0], &Connect3to1[3][1], 24},	
};

static GeoIndexData connect_level_3_to_0[4]= 
{
	{Connect3to0[0][0], &Connect3to0[0][1], 21},	
	{Connect3to0[1][0], &Connect3to0[1][1], 21},	
	{Connect3to0[2][0], &Connect3to0[2][1], 21},	
	{Connect3to0[3][0], &Connect3to0[3][1], 21},	
};
}
#endif