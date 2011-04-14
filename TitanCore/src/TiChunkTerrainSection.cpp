#include "TitanStableHeader.h"
#include "TiChunkTerrainSection.h"
#include "TiChunkTerrain.h"
#include "HardwareBufferManager.h"
#include "TitanRenderQueue.h"
#include "ConsoleDebugger.h"

namespace Titan
{
#define LEVEL_SIDE_LENGTH(i) (1<<i)
#define LEVEL_COUNT(i) (LEVEL_SIDE_LENGTH(i)*LEVEL_SIDE_LENGTH(i))
#define SKIRT_HEIGHT	50.0f

	ChunkTerrainSection::ChunkTerrainSection()
		:BaseTerrainSection(), mRendBaseVertex(0), mRendLevel(0)
	{

	}
	//-------------------------------------------------------------------------------//
	ChunkTerrainSection::~ChunkTerrainSection()
	{
		for(uint i = 0; i < CHUNK_MAX_LOD; ++i)
		{
			delete [] mErrorMerticTree[i];
		}
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ, uint16 heightMapX, uint16 heightMapZ, uint16 xVerts, uint16 zVerts, const Rect2D& worldRect)
	{
		BaseTerrainSection::create(creator, sectorX, sectorZ, heightMapX, heightMapZ, xVerts, zVerts, worldRect);

		ChunkTerrain* pChunkTerrain = static_cast<ChunkTerrain*>(creator);
		mTotalLevels = pChunkTerrain->getDetailLevels();

		for(uint i = 0; i < CHUNK_MAX_LOD; ++i)
		{
			mErrorMerticTree[i] = new float[LEVEL_COUNT(i)];
		}
			
		// build the errorMetric trees
		_buildErrorMetricTree();

	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::_updateRenderQueue(RenderQueue* queue, Camera* cam)
	{
		if(cam->isVisible(mWorldBound))
		{
			TerrainSectionRendVec::iterator it = mTerrainSectionRendVec.begin(), itend = mTerrainSectionRendVec.end();
			for(; it != itend; ++it)
			{
				TITAN_DELETE(*it);
			}
			mTerrainSectionRendVec.clear();
			_calcLod(cam);
			//if(mTerrainSectionRend)
			TerrainSectionRendVec::iterator it2 = mTerrainSectionRendVec.begin(), itend2 = mTerrainSectionRendVec.end();
			for(; it2 != itend2; ++it2)
			{

				queue->addRenderable(*it2);
			}
		}
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::_buildErrorMetricTree()
	{
		// the sector shift tells us how large our 
		// root node is in terms of vertices
		int shift = mCreator->getSectorShift();
		int stride = (1 << shift) + 1;

		// this information is used to setup our initial
		// step size and vertex count information
		int stepSize = stride >> CHUNK_MIN_TESSLLATION_SHIFT;
		int vertCount = (1 << CHUNK_MIN_TESSLLATION_SHIFT) + 1;

		// we can now step through the levels
		// of detail and determine an error
		// metric for each node of the quad
		// tree. This data is stored in the
		// error metric tree for later use
		for(int i = mTotalLevels - 1; i >= 0; --i)
		{
			int localStep = stepSize >> i;
			int xSpan = (vertCount - 1) * localStep;
			int zSpan = (vertCount - 1) * localStep;

			int side_count = LEVEL_SIDE_LENGTH(i);

			for(int z = 0; z < side_count; ++z)
			{
				for(int x = 0; x < side_count; ++x)
				{
					// compute the local errorMetric.
					// m_heightMapX and m_heightMapZ
					// are the pixel location in the
					// height map for this section 
					float errorMetric = mCreator->computeErrorMetricOfGrid(
						vertCount, 
						vertCount, 
						localStep - 1, 
						localStep - 1, 
						mHeightMapX + (x * xSpan), 
						mHeightMapZ + (z * zSpan));

					if(i + 1 < mTotalLevels)
					{
						int nextLevel = i + 1;
						int nX = x << 1;
						int nZ = z << 1;
						int dim = side_count << 1;

						errorMetric = maximum(
							errorMetric,
							mErrorMerticTree[nextLevel][(nZ*dim)+nX]);
						errorMetric = maximum(
							errorMetric,
							mErrorMerticTree[nextLevel][(nZ*dim)+nX+1]);
						errorMetric = maximum(
							errorMetric,
							mErrorMerticTree[nextLevel][((nZ+1)*dim)+nX]);
						errorMetric = maximum(
							errorMetric,
							mErrorMerticTree[nextLevel][((nZ+1)*dim)+nX+1]);
					}

					mErrorMerticTree[i][(z*side_count)+x] = 
						errorMetric;
				}
			}
		}
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::_buildVertexBuffer()
	{
		mWorldBound.getMinimum().y = MAX_REAL32;
		mWorldBound.getMaximum().y = MIN_REAL32;

		uint32 pageSize = mXVerts * mZVerts;
		uint32 bufferSize = pageSize << 1;
		BaseTerrain::SectorVertex* pVerts = 
			new BaseTerrain::SectorVertex[bufferSize];

		for (uint16 z = 0; z < mZVerts; ++z)
		{
			for (uint16 x = 0; x < mXVerts; ++x)
			{
				float height = 
					mCreator->readWorldHeight(
					mHeightMapX + x, 
					mHeightMapZ + z);
#if 0
				pVerts[(z * mXVerts) + x].normal = 
					mCreator->readWorldNormal(
					mHeightMapX + x, 
					mHeightMapZ + z);
#endif
				uint vertIndex = (z * mXVerts) + x;
				pVerts[vertIndex].height = height;

				height -= SKIRT_HEIGHT;
				pVerts[vertIndex + pageSize].height = height;

				mWorldBound.getMinimum().y = 
					minimum(mWorldBound.getMinimum().y, height);
				mWorldBound.getMaximum().y = 
					maximum(mWorldBound.getMaximum().y, height);
			}
		}

		mSectorVerts = HardwareBufferManager::getSingletonPtr()->createVertexBuffer(sizeof(BaseTerrain::SectorVertex), bufferSize, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false);
		mSectorVerts->writeData(0, bufferSize * sizeof(BaseTerrain::SectorVertex), (void*)pVerts);

		delete  [] pVerts;

		mVertexBufferBinding = HardwareBufferManager::getSingletonPtr()->createVertexBufferBinding();
		mVertexBufferBinding->setBinding(0, mCreator->getHorzVertexData());
		mVertexBufferBinding->setBinding(1, mSectorVerts);
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::_buildRenderData(RenderData* rend)
	{

	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::_calcLod(Camera* cam)
	{
		// compute a 2d point for each corner of the section
		Vector2 corner0(mWorldBound.getMinimum().x, mWorldBound.getMinimum().z);
		Vector2 corner1(mWorldBound.getMinimum().x, mWorldBound.getMaximum().z);
		Vector2 corner2(mWorldBound.getMaximum().x, mWorldBound.getMaximum().z);
		Vector2 corner3(mWorldBound.getMaximum().x, mWorldBound.getMinimum().z);

		Vector2 viewPoint= Vector2(cam->getPosition().x, cam->getPosition().z);

		// compute view distance to our 4 corners
		float distance0 = viewPoint.distance(corner0);
		float distance1 = viewPoint.distance(corner1);
		float distance2 = viewPoint.distance(corner2);
		float distance3 = viewPoint.distance(corner3);
		
		_recursiveTessellate(distance0, distance1, distance2, distance3, 
			0, 0, 0, 
			mCreator->getLodErrorScale(), mCreator->getRatioLimit());

	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrainSection::_recursiveTessellate(float distA, float distB, float distC, float distD, int level, int levelX, int levelZ, float vScale, float vLimit)
	{
		bool split = false;
		// can we attempt to split?
		if (level+1 < mTotalLevels)
		{
			int index = (levelZ*LEVEL_SIDE_LENGTH(level))+levelX;
			float errorMetric = mErrorMerticTree[level][index];

			// find the shortest distance
			float dist = minimum(distA, distB);
			dist = minimum(dist, distC);
			dist = minimum(dist, distD);

			// find the ratio of errorMetric over distance
			float vRatio = (errorMetric*vScale)/(dist+0.0001f);

			// if we exceed the ratio limit, split
			if (vRatio > vLimit)
			{
				int nextLevel = level+1;
				int startX = levelX<<1;
				int startZ = levelZ<<1;

				// compute midpoint distances
				float midAB = (distA + distB)*0.5f;
				float midBC = (distB + distC)*0.5f;
				float midCD = (distC + distD)*0.5f;
				float midDA = (distD + distA)*0.5f;
				float midQuad = (distA + distC)*0.5f;

				// recurse through the four children
				_recursiveTessellate(
					distA, midAB, midQuad, midDA,
					nextLevel, startX, startZ,
					vScale, vLimit);

				_recursiveTessellate(
					midAB, distB, midBC, midQuad,
					nextLevel, startX, startZ+1,
					vScale, vLimit);

				_recursiveTessellate(
					midBC, distC, midCD, midQuad,
					nextLevel, startX+1, startZ+1,
					vScale, vLimit);

				_recursiveTessellate(
					midAB, midQuad, midCD, distD,
					nextLevel, startX+1, startZ,
					vScale, vLimit);

				// remember that we split
				split = true;

			}
		}

		// did we split?
		if (!split)
		{
			// add ourselves to the renderable list
			uint8 lodShift = 5 - level;
			uint8 offsetX = levelX << lodShift;
			uint8 offsetZ = levelZ << lodShift;
			uint16 vertexStride = (1 << mCreator->getSectorShift()) + 1;
			uint16 rendBaseVertex = (offsetZ * vertexStride) + offsetX;

			ChunkTerrain* cTerrain = static_cast<ChunkTerrain*>(mCreator);
			TerrainSectionRend *chunkRend = TITAN_NEW TerrainSectionRend(this);
			int vertexCount = mCreator->getSectorVertex() * mCreator->getSectorVertex();
			RenderData* rend;
			rend = chunkRend->getRenderData();
			rend->operationType = RenderData::OT_TRIANGLE_STRIP;
			rend->vertexData = TITAN_NEW VertexData(mCreator->getVertexDecl(),mVertexBufferBinding);
			rend->vertexData->vertexStart = rendBaseVertex;
			rend->vertexData->vertexCount = vertexCount;
			rend->useIndex = true;
			rend->indexData = TITAN_NEW IndexData();
			rend->indexData->indexBuffer = cTerrain->getLodIndexBuffer(0, level);
			rend->indexData->indexStart = 0;
			rend->indexData->indexCount = rend->indexData->indexBuffer->getNumIndexes();
			mTerrainSectionRendVec.push_back(chunkRend);


			TerrainSectionRend *skirtRend = TITAN_NEW TerrainSectionRend(this);
			rend = skirtRend->getRenderData();
			rend->operationType = RenderData::OT_TRIANGLE_STRIP;
			rend->vertexData = TITAN_NEW VertexData(mCreator->getVertexDecl(),mVertexBufferBinding);
			rend->vertexData->vertexStart = rendBaseVertex;
			rend->vertexData->vertexCount = vertexCount;
			rend->useIndex = true;
			rend->indexData = TITAN_NEW IndexData();
			rend->indexData->indexBuffer = cTerrain->getLodIndexBuffer(1, level);
			rend->indexData->indexStart = 0;
			rend->indexData->indexCount = rend->indexData->indexBuffer->getNumIndexes();
			mTerrainSectionRendVec.push_back(skirtRend);
		}

	}
}