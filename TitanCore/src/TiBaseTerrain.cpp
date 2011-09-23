#include "TitanStableHeader.h"
#include "TiBaseTerrain.h"
#include "TiHardwareBufferMgr.h"
#include "TiBaseTerrainSection.h"
#include "TiRect2D.h"
#include "TiHardwareBuffer.h"
#include "TiPixelFormat.h"
#include "TiMaterialMgr.h"
#include "TiResourceGroupMgr.h"
#include "TiShaderParams.h"
#include "TiShaderMgr.h"
#include "TiImage.h"

namespace Titan
{

	BaseTerrain::BaseTerrain()
		:mSectorArray(NULL), mRootNode(NULL), mVertexDecl(NULL), mHeightTable(NULL),
		mNormalTable(NULL), mLodScale(1.33f), mRatioLimit(0.03f)
	{

	}
	//-------------------------------------------------------------------------------//
	BaseTerrain::~BaseTerrain()
	{
		destroy();
	}
	//-------------------------------------------------------------------------------//
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
		
		_buildHorzVertexData();
		_buildVertexDecl();
		_buildIndexData();
		_createTerrainSections();
		//_setupTerrainMaterial();
	}
	//-------------------------------------------------------------------------------//
	void BaseTerrain::destroy()
	{
		for(int i = 0; i < mSectorCountX * mSectorCountZ; ++i)
		{
			TITAN_DELETE mSectorArray[i];
		}
		if (mSectorArray)
			TITAN_DELETE [] mSectorArray;

		if(mHeightTable)
			delete [] mHeightTable;

		if (mNormalTable)
			delete [] mNormalTable;

	}
	//-------------------------------------------------------------------------------//
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

		for(int z=0; z<maxZ; ++z)
		{
			for(int x=0; x<maxX; ++x)
			{
				int iHeight = pHeightMap[(z * lockedRect.rowPitch * 4)+(x * 4)];
				float fHeight = (iHeight * mMapScale.y) + mWorldBound.getMinimum().y;

				mHeightTable[(z*maxX)+x] = fHeight;
			}
		}
		heightMap->unlockRect(0);

		//add normal here
		for (int z = 0; z < maxZ; ++z)
		{
			for (int x = 0; x < maxX; ++x)
			{
				// The ? : and ifs are necessary for the border cases.
				float sx = readWorldHeight(x < maxX-1 ? x+1 : x,z) - readWorldHeight(x>0 ? x-1 : x, z);
				if (x == 0 || x == maxX-1)
					sx *= 2;

				float sz = readWorldHeight(x, z<maxZ-1 ? z+1 : z) - readWorldHeight(x, z>0 ?  z-1 : z);
				if (z == 0 || z == maxZ-1)
					sz *= 2;

				mNormalTable[z*maxX+x] = Vector3(-sx*mMapScale.y, 2*mMapScale.x, sz*mMapScale.y);
				mNormalTable[z*maxX+x].normalise();
			}
		} 

	}
	//-------------------------------------------------------------------------------//
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
		mHorzVertexData = HardwareBufferMgr::getSingletonPtr()->createVertexBuffer(
			sizeof(LocalVertex), 
			mSectorVerts * mSectorVerts, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		mHorzVertexData->writeData(0, mSectorVerts * mSectorVerts * sizeof(LocalVertex), (void*)pVerts);
	
		delete [] pVerts;
	}
	//-------------------------------------------------------------------------------//
	void BaseTerrain::_buildVertexDecl()
	{
		mVertexDecl = HardwareBufferMgr::getSingletonPtr()->createVertexDeclaration();
		mVertexDecl->addElement(0, 0, VET_FLOAT2, VES_POSITION, 0);
		mVertexDecl->addElement(0, 8, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
		mVertexDecl->addElement(1, 0, VET_FLOAT1, VES_POSITION, 1);
		mVertexDecl->addElement(1, 4, VET_FLOAT3, VES_NORMAL, 0);
		//to do?
	}
	//-------------------------------------------------------------------------------//
	void BaseTerrain::_buildIndexData()
	{
#if 1
		int total_strips = mSectorVerts-1;
		int total_indexes_per_strip = mSectorVerts<<1;
		int total_indexes = (total_indexes_per_strip * total_strips) + (total_strips<<1) - 2;
		mIndexData = HardwareBufferMgr::getSingletonPtr()->createIndexBuffer(total_indexes, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		mIndexData->createSingleStripGrid(mSectorVerts, mSectorVerts, 1, 1, mSectorVerts);
#else
		int total_lists = mSectorVerts - 1;
		int total_indexes_per_list = (mSectorVerts - 1) * 6;
		int total_indexes = total_lists * total_indexes_per_list;
		mIndexData = HardwareBufferMgr::getSingletonPtr()->createIndexBuffer(total_indexes, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
		mIndexData->createTriangleListGrid(mSectorVerts, mSectorVerts, 1, 1, mSectorVerts);
#endif


	}
	//-------------------------------------------------------------------------------//
	void BaseTerrain::_createTerrainSections()
	{
		mSectorArray = new BaseTerrainSection*[mSectorCountX * mSectorCountZ];
		for(int i = 0; i < mSectorCountX * mSectorCountZ; ++i)
		{
			mSectorArray[i] = TITAN_NEW BaseTerrainSection();
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
	void BaseTerrain::_setupTerrainMaterial()
	{
		//temp, change later
		mMaterial = MaterialMgr::getSingleton().create("baseTerranMtrl").second;
		Pass* pass = mMaterial->createPass();
		pass->setPolygonMode(PM_WIREFRAME);

		char terrainVS[] = "\
		void terrainVS(\
			float2	inPos		: POSITION0,\
			float2	inUV		: TEXCOORD0,\
			float	inHeight	: POSITION1,\
			uniform float4x4 WorldViewProj,\
			out float4 oPos	: POSITION\
			)\
		{\
			oPos = mul(float4(inPos.x, inHeight, inPos.y, 1.0), WorldViewProj);\
		}";
		ShaderPtr pShader = ShaderMgr::getSingleton().create("terrainVS", mMaterial->getGroup(), ST_VERTEX_SHADER);
		pShader->setSingleParam("src", terrainVS);
		pShader->setSingleParam("target", "vs_2_0");
		pShader->setSingleParam("entry", "terrainVS");
		ShaderParamsPtr pParams = pShader->getShaderParams();
		pParams->setNamedAutoConstant("WorldViewProj", ShaderParams::ACT_WORLDVIEWPROJ_MATRIX);
		pass->setVertexShader("terrainVS");
	}
	//------------------------------------------------------------------------------//
	void BaseTerrain::setMaterial(const String& name)
	{
		mMaterial = MaterialMgr::getSingleton().load(name);
	}
	//-------------------------------------------------------------------------------//
	float BaseTerrain::readWorldHeight(uint16 mapX, uint16 mapZ) const
	{
		if (mapX >= mTableWidth) mapX = mTableWidth-1;
		if (mapZ >= mTableHeight) mapZ = mTableHeight-1;

		return mHeightTable[(mapZ * mTableWidth) + mapX];
	}
	//-------------------------------------------------------------------------------//
	float BaseTerrain::readWorldHeight(uint16 mapIndex) const
	{
		if(mapIndex < mTableWidth * mTableHeight)
		{
			return mHeightTable[mapIndex];
		}
		else
		{
			//should I use assert here?
			TITAN_EXCEPT_INVALIDPARAMS(
				"The index is valid");
			return -1.0f;
		}
	}
	//-------------------------------------------------------------------------------//
	const Vector3& BaseTerrain::readWorldNormal(uint16 mapX, uint16 mapZ) const
	{
		if (mapX >= mTableWidth) mapX = mTableWidth - 1;
		if (mapZ >= mTableHeight) mapZ = mTableHeight - 1;

		return mNormalTable[(mapZ * mTableWidth) + mapX];
	}
	//------------------------------------------------------------------------------//
	float BaseTerrain::calcMapHeight(float mapX, float mapZ)const
	{
		float fMapX = mapX * (mTableWidth-1);
		float fMapZ = mapZ * (mTableHeight-1);

		int iMapX0 = realToInt32_chop(fMapX);
		int iMapZ0 = realToInt32_chop(fMapZ);

		fMapX -= iMapX0;
		fMapZ -= iMapZ0;

		iMapX0 = clamp(iMapX0, 0, mTableWidth-1);
		iMapZ0 = clamp(iMapZ0, 0, mTableHeight-1);

		int iMapX1 = clamp(iMapX0+1, 0, mTableWidth-1);
		int iMapZ1 = clamp(iMapZ0+1, 0, mTableHeight-1);

		// read 4 map values
		float h0 = readWorldHeight(iMapX0, iMapZ0);
		float h1 = readWorldHeight(iMapX1, iMapZ0);
		float h2 = readWorldHeight(iMapX0, iMapZ1);
		float h3 = readWorldHeight(iMapX1, iMapZ1);

		float avgLo = (h1*fMapX) + (h0*(1.0f-fMapX));
		float avgHi = (h3*fMapX) + (h2*(1.0f-fMapX));

		return (avgHi*fMapZ) + (avgLo*(1.0f-fMapZ));;
	}

	void BaseTerrain::calcMapNormal(Vector3& normal, float mapX, float mapZ)const
	{
		float fMapX = mapX * (mTableWidth-1);
		float fMapZ = mapZ * (mTableHeight-1);

		int iMapX0 = realToInt32_chop(fMapX);
		int iMapZ0 = realToInt32_chop(fMapZ);

		fMapX -= iMapX0;
		fMapZ -= iMapZ0;

		iMapX0 = clamp(iMapX0, 0, mTableWidth-1);
		iMapZ0 = clamp(iMapZ0, 0, mTableHeight-1);

		int iMapX1 = clamp(iMapX0+1, 0, mTableWidth-1);
		int iMapZ1 = clamp(iMapZ0+1, 0, mTableHeight-1);

		// read 4 map values
		Vector3 h0 = readWorldNormal(iMapX0, iMapZ0);
		Vector3 h1 = readWorldNormal(iMapX1, iMapZ0);
		Vector3 h2 = readWorldNormal(iMapX0, iMapZ1);
		Vector3 h3 = readWorldNormal(iMapX1, iMapZ1);

		Vector3 avgLo = (h1*fMapX) + (h0*(1.0f-fMapX));
		Vector3 avgHi = (h3*fMapX) + (h2*(1.0f-fMapX));

		normal= (avgHi*fMapZ) + (avgLo*(1.0f-fMapZ));
		normal.normalise();
	}
	//-------------------------------------------------------------------------------//
	float BaseTerrain::computeErrorMetricOfGrid(uint16 xVerts, uint16 zVerts, uint16 xStep, uint16 zStep, uint16 xOffset, uint16 zOffset)
	{
		float result = 0.0f;
		int total_rows = zVerts - 1;
		int total_cells = xVerts - 1;

		unsigned short start_vert = 
			(zOffset * mTableWidth)+xOffset;
		unsigned short lineStep = 
			zStep * mTableWidth;

		float invXStep = 1.0f/xStep;
		float invZStep = 1.0f/zStep;

		for (int j=0;j<total_rows;++j)
		{
			uint16 indexA = start_vert;
			uint16 indexB = start_vert+lineStep;
			float cornerA = readWorldHeight(indexA);
			float cornerB = readWorldHeight(indexB);

			for (int i=0; i<total_cells;++i)
			{
				// compute 2 new corner vertices
				uint16 indexC = indexA+xStep;
				uint16 indexD = indexB+xStep;

				// grab 2 new corner height values
				float cornerC = readWorldHeight(indexC);
				float cornerD = readWorldHeight(indexD);

				// setup the step values for 
				// both triangles of this cell
				float stepX0 = (cornerD-cornerA)*invXStep;
				float stepZ0 = (cornerB-cornerA)*invZStep;
				float stepX1 = (cornerB-cornerC)*invXStep;
				float stepZ1 = (cornerD-cornerC)*invZStep;

				// find the max error for all points
				// covered by the two triangles
				int subIndex = indexA;
				for (int z=0; z<zStep;++z)
				{
					for (int x=0; x<xStep;++x)
					{
						float trueHeight = readWorldHeight(subIndex);
						++subIndex;

						float intepolatedHeight;

						if (z < (xStep-x))
						{
							intepolatedHeight = cornerA + (stepX0*x) + (stepZ0*z);
						}
						else
						{
							intepolatedHeight = cornerC + (stepX1*x) + (stepZ1*z);
						}

						float delta = absoluteValue(trueHeight - intepolatedHeight);
						result = maximum(result, delta);

					}
					subIndex = indexA+(z* mTableWidth);
				}

				// save the corners for the next cell
				indexA = indexC;
				indexB = indexD;
				cornerA = cornerC;
				cornerB = cornerD;
			}

			start_vert += lineStep;
		}

		return result;
	}
	//----------------------------------------------------------------------------//
	inline const MaterialPtr& BaseTerrain::getMaterial() const 
	{
		return mMaterial;
	}
	//------------------------------------------------------------------------------//
	static float computeWeight(float value, float minExtent, float maxExtent)
	{
		float weight = 0.0f;

		if (value >= minExtent
			&& value <= maxExtent)
		{
			float span = maxExtent - minExtent;
			weight = value - minExtent;

			// convert to a 0-1 range value
			// based on its distance to the midpoint
			// of the range extents
			weight *= 1.0f/span; 
			weight -= 0.5f;
			weight *= 2.0f;

			// square the result for non-linear falloff
			weight *= weight;

			// invert and bound-check the result
			weight = 1.0f-absoluteValue(weight);
			weight = clamp(weight, 0.001f, 1.0f);
		}

		return weight;
	}
	//------------------------------------------------------------------------------//
	void BaseTerrain::genBlendImage(Image& img, const elevationDataVec& elevDataVec)
	{
		bool success = false;
		uint x,y,i;

		// make sure there are no more than 4 structures
		uint elevationDataCount = elevDataVec.size();
		if(elevationDataCount> 4)
			elevationDataCount = 4;

		// get the blend image dimensions
		uint image_width = img.getWidth();
		uint image_height = img.getHeight();

		// compute the step values for uv
		// coordinates across the image
		float uStep = 1.0f/(image_width-1);
		float vStep = 1.0f/(image_height-1);

		// these 4 mask values control 
		// which color component of the
		// blend image we write to
		Vector4 mask[4];
		mask[0] = Vector4(1.0f,0.0f,0.0f,0.0f);
		mask[1] = Vector4(0.0f,1.0f,0.0f,0.0f);
		mask[2] = Vector4(0.0f,0.0f,1.0f,0.0f);
		mask[3] = Vector4(0.0f,0.0f,0.0f,1.0f);

		uchar*	pImageData = TITAN_NEW_ARRAY_T(uchar, image_width * image_height * img.getBytesPerPixel(), MEMCATEGORY_GENERAL);

		uint8*	pData = pImageData;
		// step through and generate each pixel
		for (y=0; y<image_height; ++y)
		{
			for (x=0; x<image_width; ++x)
			{
				float totalBlend = 0.0f;
				Vector4 blendFactors(0.0f, 0.0f, 0.0f, 0.0f);

				// get the elevation and surface normal
				float u = x*uStep;
				float v = y*vStep;
				float map_height = calcMapHeight(u,v);
				Vector3 normal;
				calcMapNormal(normal, u, v);

				// examine each elevationData structure
				// a compute a weight for each one
				for (i = 0; i < elevationDataCount; ++i)
				{
					// compute a weight based on elevation
					float elevationScale = computeWeight(map_height, elevDataVec[i].minElevation, elevDataVec[i].maxElevation);

					// compute a weight based on slope
					float slopeScale = computeWeight(normal.y, elevDataVec[i].minNormalZ, elevDataVec[i].maxNormalZ);

					// combine the two with the relative
					// strength of this surface type
					float scale = elevDataVec[i].strength * elevationScale * slopeScale;

					// write the result to the proper
					// channel of the blend factor vector
					blendFactors += mask[i]*scale;

					// and remember the total weight
					totalBlend += scale;
				}

				// balance the data (so they add up to 255)
				float blendScale = 255.0f /totalBlend;

				// now compute the actual color by
				// multiplying each channel 
				// by the blend scale
				blendFactors *= blendScale;

				// clamp and convert to color values
				uint8 r = (uint8) clamp(blendFactors.x, 0.0f, 255.0f);
				uint8 g = (uint8) clamp(blendFactors.y, 0.0f, 255.0f);
				uint8 b = (uint8) clamp(blendFactors.z, 0.0f, 255.0f);
				uint8 a = (uint8) clamp(blendFactors.w, 0.0f, 255.0f);
				
				*pData++ = b;
				*pData++ = g;
				*pData++ = r;
				*pData++ = a;
				// pack and write a 32bit pixel value
				//uint32 color = (a<<24)+(r<<16)+(g<<8)+b;
			}
		}
		img.loadRawData(pImageData, image_width, image_height, PF_A8R8G8B8);
	
	}

}