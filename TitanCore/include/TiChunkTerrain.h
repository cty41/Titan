#ifndef __TITAN_CHUNK_TERRAIN_HH
#define __TITAN_CHUNK_TERRAIN_HH

#include "TitanPrerequisites.h"
#include "TitanBaseTerrain.h"

namespace Titan
{
#define  CHUNK_INDEX_TYPE	2
#define	 CHUNK_MAX_LOD		3
#define	 CHUNK_MIN_TESSLLATION_SHIFT 2

	class _DllExport ChunkTerrain : public BaseTerrain
	{
	public:
		ChunkTerrain();

		~ChunkTerrain();

		virtual void create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift  = 5);

		virtual void destroy();

		uint32	getDetailLevels() const { return mDetailLevels;}
		
		IndexBufferSharedPtr	getLodIndexBuffer(uint8 indexType, uint8 lod);

	protected:

		virtual void _createTerrainSections();

		virtual	void _buildHorzVertexData();

		virtual void _buildIndexData();

	protected:
		ChunkTerrainSection*	mChunkArray;
		IndexBufferSharedPtr	mLODIndexArray[CHUNK_INDEX_TYPE][CHUNK_MAX_LOD];
		uint32		mDetailLevels;
	};
}
#endif