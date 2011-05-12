#include "TitanStableHeader.h"
#include "TiChunkTerrain.h"
#include "TiChunkTerrainSection.h"
#include "TiHardwareBufferMgr.h"

namespace Titan
{
	ChunkTerrain::ChunkTerrain()
		:BaseTerrain()
	{

	}
	//-------------------------------------------------------------------------------//
	ChunkTerrain::~ChunkTerrain()
	{
		destroy();
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrain::create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift /* = 5 */)
	{
		// chunk terrain sections 
		// are forced to be 16x16 cells
		shift = 5;

		BaseTerrain::create(rootNode, heightMap, worldBound, shift);
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrain::destroy()
	{

	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrain::_createTerrainSections()
	{
		mSectorArray = new BaseTerrainSection*[mSectorCountX * mSectorCountZ];
		for(int i = 0; i < mSectorCountX * mSectorCountZ; ++i)
		{
			mSectorArray[i] = TITAN_NEW ChunkTerrainSection();
		}

		for (int z=0; z<mSectorCountZ; ++z)
		{
			for (int x=0; x<mSectorCountX; ++x)
			{
				Vector2 sectorPos(
					mWorldBound.getMinimum().x + (x * mSectorSize.x),
					mWorldBound.getMinimum().z + (z * mSectorSize.y));

				Rect2D sectorRect(
					sectorPos.x, sectorPos.y,
					sectorPos.x + mSectorSize.x, sectorPos.y + mSectorSize.y);

				uint16 xPixel = x<<mSectorShift;
				uint16 zPixel = z<<mSectorShift;

				uint16 index = (z*mSectorCountX)+x;

				mRootNode->attachObject(mSectorArray[index]);
				mSectorArray[index]->create(this, x, z, xPixel, zPixel, 
					mSectorVerts, 
					mSectorVerts, 
					sectorRect);
			}
		}

	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrain::_buildHorzVertexData()
	{
		Vector2	cellSize(mSectorSize.x / mSectorUnits,
			mSectorSize.y / mSectorUnits);

		int pageSize = (mSectorVerts * mSectorVerts);
		int bufferSize = pageSize<<1;

		Vector2 vert(0.0f, 0.0f);
		LocalVertex* pVerts = new LocalVertex[bufferSize];

		// fill the vertex stream with horizontal position infos and
		// uv coordinates. All other data (height and
		// surface normals) are stored in the vertex
		// buffers of each terrain section
		for (int z=0; z<mSectorVerts; ++z)
		{
			vert.x = 0.0f;
			vert.y = z * cellSize.y;

			for (int x=0; x<mSectorVerts; ++x)
			{
				int index = (z*mSectorVerts)+x;
				Vector2 UV((float)x/(float)(mSectorVerts-1),
					(float)z/(float)(mSectorVerts-1));
				pVerts[index].xzPos = vert;
				pVerts[index].localUV = UV;

				// duplicate this data into the
				// second page as well
				pVerts[index+pageSize].xzPos = vert;
				pVerts[index+pageSize].localUV = UV;

				vert.x += cellSize.x;
			}
		}

		// now that we have built the data,
		// create one of our vertex buffer
		// resource objects with it
		mHorzVertexData = HardwareBufferMgr::getSingletonPtr()->createVertexBuffer(sizeof(LocalVertex), bufferSize, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		mHorzVertexData->writeData(0, bufferSize * sizeof(LocalVertex), (void*)pVerts);

		delete [] pVerts;
	}
	//-------------------------------------------------------------------------------//
	void ChunkTerrain::_buildIndexData()
	{


		int stride = (1 << mSectorShift) + 1;
		int stepSize = stride >> CHUNK_MIN_TESSLLATION_SHIFT;
		int vertCount = (1 << CHUNK_MIN_TESSLLATION_SHIFT) + 1;

		int total_strips = 
			vertCount-1;
		int total_indexes_per_strip = 
			vertCount<<1;

		int total_indexes = 
			(total_indexes_per_strip * total_strips) 
			+ (total_strips<<1) - 2;

		mDetailLevels = 0;

		while (stepSize && mDetailLevels < CHUNK_MAX_LOD)
		{
			mLODIndexArray[0][mDetailLevels] = HardwareBufferMgr::getSingletonPtr()->createIndexBuffer(total_indexes, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
			mLODIndexArray[0][mDetailLevels]->createSingleStripGrid(vertCount, vertCount, stepSize, stepSize, stride);

			stepSize >>= 1;
			++mDetailLevels;
		}

		// build an index buffer for each skirt.
		// Consider each square detail level of
		// the tile as a square with corners
		// A,B,C & D as in the figure below

		/*
		D -------- C
		^	|          |
		|	|          |
		p	|          |
		o	|          |
		s	|          |
		A -------- B
		y
		pos x ->
		*/
		// the following code generates a single strip 
		// for each skirt following the edges
		// AB, BC, CD and DA
		int sideLength = (1 << mSectorShift) + 1;
		int pageSize = sideLength * sideLength;

		for(uint32 iLevel = 0; iLevel < mDetailLevels; ++iLevel)
		{
			int skirtSide = (1 << CHUNK_MIN_TESSLLATION_SHIFT) + 1;
			int indexCount = skirtSide << 3;
			uint16* indexList = new uint16[indexCount];
			uint16* pIndex = indexList;

			int vStep;
			int vIndex;
			int count;
			int horzStep = (sideLength>>iLevel)>> CHUNK_MIN_TESSLLATION_SHIFT;
			int vertStep = sideLength*horzStep;

			// side AB
			vIndex=0;
			vStep = vertStep;
			for(count=0; count<skirtSide;++count)
			{
				*(pIndex++)=vIndex;
				*(pIndex++)=vIndex+pageSize;
				vIndex +=vStep;
			}

			// side BC
			vIndex -= vStep;
			vStep = horzStep;
			for(count=0; count<skirtSide;++count)
			{
				*(pIndex++)=vIndex;
				*(pIndex++)=vIndex+pageSize;
				vIndex +=vStep;
			}

			// side CD
			vIndex -= vStep;
			vStep = -vertStep;
			for(count=0; count<skirtSide;++count)
			{
				*(pIndex++)=vIndex;
				*(pIndex++)=vIndex+pageSize;
				vIndex +=vStep;
			}

			// side DA
			vIndex -= vStep;
			vStep = -horzStep;
			for(count=0; count<skirtSide;++count)
			{
				*(pIndex++)=vIndex;
				*(pIndex++)=vIndex+pageSize;
				vIndex +=vStep;
			}

			mLODIndexArray[1][iLevel] = HardwareBufferMgr::getSingletonPtr()->createIndexBuffer(indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
			mLODIndexArray[1][iLevel]->writeData(0, indexCount, indexList);

			delete [] indexList;
		}



	}
	//-------------------------------------------------------------------------------//
	IndexBufferPtr ChunkTerrain::getLodIndexBuffer(uint8 indexType, uint8 lod)
	{
		assert(indexType < 2 && lod < CHUNK_MAX_LOD);
		return mLODIndexArray[indexType][lod];
	}
}