#ifndef __TITAN_CHUNKTERRAIN_SECTION__HH
#define __TITAN_CHUNKTERRAIN_SECTION__HH

#include "TiPrerequisites.h"
#include "TitanBaseTerrainSection.h"

namespace Titan
{
#define  CHUNK_INDEX_TYPE	2
#define	 CHUNK_MAX_LOD		3
#define	 CHUNK_MIN_TESSLLATION_SHIFT 2
	class _DllExport ChunkTerrainSection : public BaseTerrainSection
	{
	public:
		ChunkTerrainSection();

		~ChunkTerrainSection();

		virtual void		create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ, uint16 heightMapX, uint16 heightMapZ, uint16 xVerts, uint16 zVerts, const Rect2D& worldRect);

		virtual void		_updateRenderQueue(RenderQueue* queue, Camera* cam);


	protected:
		virtual void		_buildVertexBuffer();
		
		virtual void		_buildRenderData(RenderData* rend);

		void				_buildErrorMetricTree();

		void				_calcLod(Camera* cam);

		// recursive functions used internally
		void				_recursiveTessellate(float distA, float distB, float distC, float distD,
									int level, int levelX, int levelZ, float vScale, float vLimit);



	protected:
		float*		mErrorMerticTree[CHUNK_MAX_LOD];
		uint16		mTotalLevels;

		//use for adding renderable 
		uint16		mRendBaseVertex;
		uint8		mRendLevel;
		VertexData*	mVertxData;
		IndexData*	mChunkIndexData;
		IndexData*	mSkirtIndexData;
	};
}

#endif