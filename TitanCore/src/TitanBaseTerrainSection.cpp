#include "TitanStableHeader.h"
#include "TitanBaseTerrainSection.h"
#include "TitanBaseTerrain.h"
#include "HardwareBufferManager.h"
#include "TitanRenderQueue.h"

namespace Titan
{
	static const TexturePtr sNullTexPtr;


	BaseTerrainSection::BaseTerrainSection()
		: mCreator(NULL), mXVerts(0), mZVerts(0), mTerrainSectionRend(NULL), mVertexBufferBinding(NULL)
	{
		mType = "BaseTerrainSection";
		mWorldBound.setFinite();
	}
	//-------------------------------------------------------------------------------//
	BaseTerrainSection::~BaseTerrainSection()
	{
		if(mTerrainSectionRend)
			TITAN_DELETE mTerrainSectionRend;
#if 0
		TerrainSectionRendVec::iterator it = mTerrainSectionRendVec.begin(), itend = mTerrainSectionRendVec.end();
		for(;it != itend; ++it)
			TITAN_DELETE (*it);
#endif
	}
	//-------------------------------------------------------------------------------//
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
	}
	//-------------------------------------------------------------------------------//
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
		
		mVertexBufferBinding = HardwareBufferManager::getSingletonPtr()->createVertexBufferBinding();
		mVertexBufferBinding->setBinding(0, mCreator->getHorzVertexData());
		mVertexBufferBinding->setBinding(1, mSectorVerts);

		mTerrainSectionRend = TITAN_NEW TerrainSectionRend(this);
		mTerrainSectionRend->_buildRenderData(this);

	}
	//-------------------------------------------------------------------------------//
	void BaseTerrainSection::_buildRenderData(RenderData* rend)
	{
		rend->operationType = RenderData::OT_TRIANGLE_STRIP;
		rend->vertexData = TITAN_NEW VertexData(mCreator->getVertexDecl(), mVertexBufferBinding);
		rend->vertexData->vertexCount = mCreator->getHorzVertexData()->getNumVertices();

		rend->useIndex = true;
		rend->indexData = TITAN_NEW IndexData();
		rend->indexData->indexBuffer = mCreator->getSharedIndexData();
		rend->indexData->indexStart = 0;
		rend->indexData->indexCount = rend->indexData->indexBuffer->getNumIndexes();
	}
	//-------------------------------------------------------------------------------//
	void BaseTerrainSection::_updateRenderQueue(RenderQueue* queue, Camera* cam)
	{
		if(cam->isVisible(mWorldBound))
		{
			if(mTerrainSectionRend)
				queue->addRenderable(mTerrainSectionRend);
#if 0
			TerrainSectionRendVec::iterator it = mTerrainSectionRendVec.begin(), itend = mTerrainSectionRendVec.end();
			for(;it != itend; ++it)
				queue->addRenderable(*it);
#endif
		}
		else
			return;
	}
	//-------------------------------------------------------------------------------//




	BaseTerrainSection::TerrainSectionRend::TerrainSectionRend(BaseTerrainSection* creator)
		:mCreator(creator)
	{

	}
	//-------------------------------------------------------------------------------//
	BaseTerrainSection::TerrainSectionRend::~TerrainSectionRend()
	{
	}
	//-------------------------------------------------------------------------------//
	float BaseTerrainSection::TerrainSectionRend::getSquaredDistance(Camera* cam)
	{
		Vector3 diff = mCreator->getWorldBound().getCenter() - cam->getPosition();
		return diff.squaredLength();
	}
	const TexturePtr& BaseTerrainSection::TerrainSectionRend::getTexture() const 
	{
		return sNullTexPtr;
	}
	//-------------------------------------------------------------------------------//
	void BaseTerrainSection::TerrainSectionRend::customUpdate()
	{
		mCreator->getCreator()->getShaderEffect()->setNamedParamByIndex(mCreator->getPosOffsetIndex(), (const float*)&mCreator->getPosOffset());
	}
	//-------------------------------------------------------------------------------//
	void BaseTerrainSection::TerrainSectionRend::_buildRenderData(BaseTerrainSection* creator)
	{
		mCreator->_buildRenderData(&mRenderData);
	}
	
}