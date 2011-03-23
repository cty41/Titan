#include "TitanStableHeader.h"
#include "TitanBaseTerrainSection.h"
#include "TitanBaseTerrain.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	BaseTerrainSection::BaseTerrainSection()
		: mCreator(NULL), mXVerts(0), mZVerts(0)
	{
		mType = "BaseTerrainSection";
		mWorldBound.setInfinite();
	}
	//-------------------------------------------------------------//
	BaseTerrainSection::~BaseTerrainSection()
	{
		destroy();
	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ, uint16 heightMapX, uint16 heightMapZ, uint16 xVerts, uint16 zVerts, const Rect2D& worldRect)
	{
		mCreator = creator;
		mXVerts = xVerts;
		mZVerts = zVerts;
		mSectorX = sectorX;
		mSectorZ = sectorZ;
		mWorldBound = worldRect;
		mHeightMapX = heightMapX;
		mHeightMapZ = heightMapZ;

		mPosOffsetIdx = mCreator->getShaderEffect()->getShaderParams().getNameParamIndex("posOffset");
		Vector2 sectorSize = mCreator->getSectorSize();
		mPosOffset = Vector4(1.0f, 1.0f, 
			mCreator->getWorldBound().getMinimum().x + sectorSize.x * mSectorX, 
			mCreator->getWorldBound().getMinimum().z + sectorSize.y * mSectorZ);

		_buildVertexBuffer();
		_buildRenderData();

	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::destroy()
	{
		if(mRenderData.indexData)
			TITAN_DELETE mRenderData.indexData;

		if (mRenderData.vertexData)
			TITAN_DELETE mRenderData.vertexData;
	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::_buildVertexBuffer()
	{
		mWorldBound.getMinimum().y = MAX_REAL32;
		mWorldBound.getMaximum().y = MIN_REAL32;

		BaseTerrain::SectorVertex* pVerts = 
			new BaseTerrain::SectorVertex[mXVerts * mZVerts];

		for (uint16 z = 0; z < mZVerts; ++z)
		{
			for (uint16 x = 0; x < mXVerts; ++x)
			{
				float height = 
					mCreator->readWorldHeight(
					mHeightMapX + x, 
					mHeightMapZ + z);
				pVerts[(z * mXVerts) + x].height = 
					height;
#if 0
				pVerts[(z * mXVerts) + x].normal = 
					mCreator->readWorldNormal(
					mHeightMapX + x, 
					mHeightMapZ + z);
#endif
				mWorldBound.getMinimum().y = 
					minimum(mWorldBound.getMinimum().y, height);
				mWorldBound.getMaximum().y = 
					maximum(mWorldBound.getMaximum().y, height);
			}
		}
		mSectorVerts = HardwareBufferManager::getSingletonPtr()->createVertexBuffer(sizeof(BaseTerrain::SectorVertex), mXVerts * mZVerts, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false);
		mSectorVerts->writeData(0, mXVerts * mZVerts * sizeof(BaseTerrain::SectorVertex), (void*)pVerts);

		delete  [] pVerts;
	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::_buildRenderData()
	{
		mRenderData.operationType = RenderData::OT_TRIANGLE_STRIP;
		mRenderData.useIndex = true;

		VertexBufferBinding* binding = HardwareBufferManager::getSingletonPtr()->createVertexBufferBinding();
		binding->setBinding(0, mCreator->getHorzVertexData());
		binding->setBinding(1, mSectorVerts);
		mRenderData.vertexData = TITAN_NEW VertexData(mCreator->getVertexDecl(), binding);
		mRenderData.vertexData->vertexCount = mCreator->getHorzVertexData()->getNumVertices();

		mRenderData.indexData = TITAN_NEW IndexData();
		mRenderData.indexData->indexBuffer = mCreator->getSharedIndexData();
		mRenderData.indexData->indexStart = 0;
		mRenderData.indexData->indexCount = mRenderData.indexData->indexBuffer->getNumIndexes();

	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::_updateRenderableList(SceneMgr::RenderableList& renderableList, Camera* cam)
	{
		if(cam->isVisible(mWorldBound))
			renderableList.push_back(this);
		else
			return;
	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::getTransformMat(Matrix4* transMat)
	{
		*transMat = _getAttachedNodeFullTransform();
	}
	//-------------------------------------------------------------//
	ShaderEffectPtr BaseTerrainSection::getShaderEffect()
	{
		return mCreator->getShaderEffect();
	}
	//-------------------------------------------------------------//
	bool BaseTerrainSection::hasShader()
	{
		return (!mCreator->getShaderEffect().isNull());
	}
	//-------------------------------------------------------------//
	void BaseTerrainSection::customUpdate()
	{

		mCreator->getShaderEffect()->setNamedParamByIndex(mPosOffsetIdx, (const float*)&mPosOffset);
		
	}
}