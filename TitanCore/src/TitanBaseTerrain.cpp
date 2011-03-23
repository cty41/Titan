#include "TitanStableHeader.h"
#include "TitanBaseTerrain.h"
#include "HardwareBufferManager.h"
#include "TitanBaseTerrainSection.h"
#include "TitanRect2D.h"
#include "HardwareBuffer.h"
#include "TitanPixelFormat.h"
#include "ShaderEffectMgr.h"
#include "TitanResourceGroupManager.h"

namespace Titan
{
	BaseTerrain::BaseTerrain()
		:mSectorArray(NULL), mRootNode(NULL), mVertexDecl(NULL), mHeightTable(NULL),
		mNormalTable(NULL)
	{}
	//-------------------------------------------------------------//
	BaseTerrain::~BaseTerrain()
	{
		destroy();
	}
	//-------------------------------------------------------------//
	void BaseTerrain::create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift )
	{
		mSectorShift = shift;
		mSectorUnits = 1<<shift;
		mSectorVerts = mSectorUnits + 1;

		mRootNode = rootNode;
		mWorldBound = worldBound;
		mWorldSize = mWorldBound.getSize();

		mTableWidth = (uint16)heightMap->getWidth();
		mTableHeight = (uint16)heightMap->getHeight();

		mMapScale = Vector3(mWorldSize.x / mTableWidth, mWorldSize.y / 255.0f, mWorldSize.z / mTableHeight);

		// convert the height map to
		// data stored in local tables
		_buildHeightAndNormalTables(heightMap);

		mSectorCountX = mTableWidth >> mSectorShift;
		mSectorCountZ = mTableHeight >> mSectorShift;

		mSectorSize = Vector2(mWorldSize.x / mSectorCountX,
			mWorldSize.z / mSectorCountZ);

		mEffect = Titan::ShaderEffectMgr::getSingletonPtr()->loadManually("single_texture_terrain.fx", Titan::ResourceGroupManager::GENERAL_RESOURCE_GROUP);
		
		_buildHorzVertexData();
		_buildVertexDecl();
		_buildIndexData();
		_createTerrainSections();


	}
	//-------------------------------------------------------------//
	void BaseTerrain::destroy()
	{
		//change later
		//TITAN_DELETE [] mSectorArray;

		if(mHeightTable)
			delete [] mHeightTable;

		if (mNormalTable)
			delete [] mNormalTable;

	}
	//-------------------------------------------------------------//
	void BaseTerrain::_buildHeightAndNormalTables(TexturePtr heightMap)
	{
		if(mHeightTable)
			delete [] mHeightTable;
		if (mNormalTable)
			delete [] mNormalTable;

		int maxZ = mTableHeight;
		int maxX = mTableWidth;

		mHeightTable = new float[maxX * maxZ];
		mNormalTable = new Vector3[maxX * maxZ];

		PixelBox	lockedRect;
		heightMap->lockRect(0, &lockedRect, NULL, HardwareBuffer::HBL_READ_ONLY);
		uint8* pHeightMap = (uint8*)lockedRect.data;
		int z,x;
		for(z=0; z<maxZ; ++z)
		{
			for(x=0; x<maxX; ++x)
			{
				int iHeight = 
					pHeightMap[(z * lockedRect.rowPitch * 4)+(x * 4)];
				float fHeight = 
					(iHeight * mMapScale.y) 
					+ mWorldBound.getMinimum().y;

				mHeightTable[(z*maxX)+x] = 
					fHeight;
			}
		}
		heightMap->unlockRect(0);

		//add normal here

	}
	//-------------------------------------------------------------//
	void BaseTerrain::_buildHorzVertexData()
	{
		Vector2	cellSize(mSectorSize.x / mSectorUnits,
			mSectorSize.y / mSectorUnits);

		Vector2 vert(0.0f, 0.0f);
		LocalVertex* pVerts = new LocalVertex[mSectorVerts * mSectorVerts];

		// fill the vertex stream with horizontal position infos and
		// uv coordinates. All other data (height and
		// surface normals) are stored in the vertex
		// buffers of each terrain section
		for (int y=0; y<mSectorVerts; ++y)
		{
			vert.x = 0.0f;
			vert.y = y * cellSize.y;

			for (int x=0; x<mSectorVerts; ++x)
			{
				pVerts[(y*mSectorVerts)+x].xzPos = vert;
				pVerts[(y*mSectorVerts)+x].localUV = Vector2((float)x/(float)(mSectorVerts-1),
					(float)y/(float)(mSectorVerts-1));

				vert.x += cellSize.x;
			}
		}
		
		// now that we have built the data,
		// create one of our vertex buffer
		// resource objects with it
		mHorzVertexData = HardwareBufferManager::getSingletonPtr()->createVertexBuffer(sizeof(LocalVertex), mSectorVerts * mSectorVerts, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		mHorzVertexData->writeData(0, mSectorVerts * mSectorVerts * sizeof(LocalVertex), (void*)pVerts);
	
		delete [] pVerts;
	}
	//-------------------------------------------------------------//
	void BaseTerrain::_buildVertexDecl()
	{
		mVertexDecl = HardwareBufferManager::getSingletonPtr()->createVertexDeclaration();
		mVertexDecl->addElement(0, 0, VET_FLOAT2, VES_POSITION, 0);
		mVertexDecl->addElement(0, 8, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
		mVertexDecl->addElement(1, 0, VET_FLOAT1, VES_POSITION, 1);
		//mVertexDecl->addElement(1, 4, VET_FLOAT3, VES_NORMAL, 0);
		//to do?
	}
	//-------------------------------------------------------------//
	void BaseTerrain::_buildIndexData()
	{
		int total_strips = 
			mSectorVerts-1;
		int total_indexes_per_strip = 
			mSectorVerts<<1;

		// the total number of indices is equal
		// to the number of strips times the
		// indices used per strip plus one
		// degenerate triangle between each strip
		int total_indexes = 
			(total_indexes_per_strip * total_strips) 
			+ (total_strips<<1) - 2;
		mIndexData = HardwareBufferManager::getSingletonPtr()->createIndexBuffer(total_indexes, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		mIndexData->createSingleStripGrid(mSectorVerts, mSectorVerts, 1, 1, mSectorVerts, 0);
	}
	//-------------------------------------------------------------//
	void BaseTerrain::_createTerrainSections()
	{
		mSectorArray = TITAN_NEW BaseTerrainSection[mSectorCountX * mSectorCountZ];

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

				mRootNode->attachObject(&mSectorArray[index]);
				mSectorArray[index].create(this, x, z, xPixel, zPixel, 
					mSectorVerts, 
					mSectorVerts, 
					sectorRect);
			

			}
		}
	}
	//-------------------------------------------------------------//
	float BaseTerrain::readWorldHeight(uint16 mapX, uint16 mapZ) const
	{
		if (mapX >= mTableWidth) mapX = mTableWidth-1;
		if (mapZ >= mTableHeight) mapZ = mTableHeight-1;

		return mHeightTable[(mapZ * mTableWidth) + mapX];
	}
	//-------------------------------------------------------------//
	float BaseTerrain::readWorldHeight(uint16 mapIndex) const
	{
		if(mapIndex < mTableWidth * mTableHeight)
		{
			return mHeightTable[mapIndex];
		}
		else
		{
			//should I use assert here?
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"The index is valid",
				"BaseTerrain::readWorldHeight");
			return -1.0f;
		}
	}
	//-------------------------------------------------------------//
	const Vector3& BaseTerrain::readWorldNormal(uint16 mapX, uint16 mapZ) const
	{
		if (mapX >= mTableWidth) mapX = mTableWidth - 1;
		if (mapZ >= mTableHeight) mapZ = mTableHeight - 1;

		return mNormalTable[(mapZ * mTableWidth) + mapX];
	}
	//-------------------------------------------------------------//

}