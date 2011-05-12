#include "TitanStableHeader.h"
#include "TiGeoTerrain.h"
#include "TiHardwareBufferMgr.h"
#include "TiGeoTerrainSection.h"

namespace Titan
{
	GeoTerrain::GeoTerrain()
		:BaseTerrain()
	{
	}
	//------------------------------------------------------------------------------//
	GeoTerrain::~GeoTerrain()
	{
	}
	//------------------------------------------------------------------------------//
	void GeoTerrain::create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift)
	{
		// terrain tiles are forced to be 8x8 cells
		shift = 3;

		BaseTerrain::create(rootNode, heightMap, worldBound, shift);

		_genDetailLevels();
	}
	//------------------------------------------------------------------------------//
	void GeoTerrain::destroy()
	{
	}
	//------------------------------------------------------------------------------//
	void GeoTerrain::_createTerrainSections()
	{
		mSectorArray = new BaseTerrainSection*[mSectorCountX * mSectorCountZ];
		for(int i = 0; i < mSectorCountX * mSectorCountZ; ++i)
		{
			mSectorArray[i] = TITAN_NEW GeoTerrainSection();
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
	//------------------------------------------------------------------------------//
	void GeoTerrain::_genDetailLevels()
	{
		// Detail Level 0 (lowest detail level)
		mDetailLevelStructs[0].tileBodies[0] = TITAN_NEW IndexBufferInfo(_createIndexStructs(base_level_0), base_level_0.flag == STRIP_LIST) ;

		// Detail Level 1-3, main body tiles
		for (int body = 0; body < 16; ++body)
		{
			mDetailLevelStructs[1].tileBodies[body] = TITAN_NEW IndexBufferInfo(_createIndexStructs(base_level_1[body]), base_level_1[body].flag == STRIP_LIST);
			mDetailLevelStructs[2].tileBodies[body] = TITAN_NEW IndexBufferInfo(_createIndexStructs(base_level_2[body]), base_level_2[body].flag == STRIP_LIST);
			mDetailLevelStructs[3].tileBodies[body] = TITAN_NEW IndexBufferInfo(_createIndexStructs(base_level_3[body]), base_level_3[body].flag == STRIP_LIST);
		}

		// create the tile connectors
		for (int side = 0; side < TotalSides; ++side)
		{
			mDetailLevelStructs[1].tileConnectors[side][0] = TITAN_NEW IndexBufferInfo(_createIndexStructs(connect_level_1_to_0[side]), connect_level_1_to_0[side].flag == STRIP_LIST);
			mDetailLevelStructs[2].tileConnectors[side][0] = TITAN_NEW IndexBufferInfo(_createIndexStructs(connect_level_2_to_0[side]), connect_level_2_to_0[side].flag == STRIP_LIST);
			mDetailLevelStructs[2].tileConnectors[side][1] = TITAN_NEW IndexBufferInfo(_createIndexStructs(connect_level_2_to_1[side]), connect_level_2_to_1[side].flag == STRIP_LIST);
			mDetailLevelStructs[3].tileConnectors[side][0] = TITAN_NEW IndexBufferInfo(_createIndexStructs(connect_level_3_to_0[side]), connect_level_3_to_0[side].flag == STRIP_LIST);
			mDetailLevelStructs[3].tileConnectors[side][1] = TITAN_NEW IndexBufferInfo(_createIndexStructs(connect_level_3_to_1[side]), connect_level_3_to_1[side].flag == STRIP_LIST);
			mDetailLevelStructs[3].tileConnectors[side][2] = TITAN_NEW IndexBufferInfo(_createIndexStructs(connect_level_3_to_2[side]), connect_level_3_to_2[side].flag == STRIP_LIST);
		}
	}
	//------------------------------------------------------------------------------//
	IndexBufferPtr GeoTerrain::_createIndexStructs(const GeoIndexData& srcData)
	{
		//error?
		IndexBufferPtr pIndex = HardwareBufferMgr::getSingletonPtr()->createIndexBuffer(srcData.vertex_count, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		pIndex->writeData(0, pIndex->getSizeInBytes(), (void*)srcData.index_table);
		
		return pIndex;
	}
	//------------------------------------------------------------------------------//
	GeoTerrainSection*	GeoTerrain::getGeoSection(size_t index) const
	{
		assert(index < mSectorCountX * mSectorCountZ && "index out of range!");
		return static_cast<GeoTerrainSection*>(mSectorArray[index]);
	}
	//------------------------------------------------------------------------------//
	const GeoTerrain::IndexStruct& GeoTerrain::getDetailLevelStruct(size_t index) const
	{
		assert(index < TOTAL_DETAIL_LEVELS && "index out of range!");
		return mDetailLevelStructs[index];
	}
}