#include "TitanStableHeader.h"
#include "TiBaseTerrainSection.h"
#include "TiBaseTerrain.h"
#include "TiHardwareBufferMgr.h"
#include "TiRenderQueue.h"

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

		Vector2 sectorSize = mCreator->getSectorSize();
		mSectionPos = Vector3(
			mCreator->getWorldBound().getMinimum().x + sectorSize.x * mSectorX, 
			1.0f, 
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

				pVerts[(z * mXVerts) + x].normal = 
					mCreator->readWorldNormal(
					mHeightMapX + x, 
					mHeightMapZ + z);

				mWorldBound.getMinimum().y = 
					minimum(mWorldBound.getMinimum().y, height);
				mWorldBound.getMaximum().y = 
					maximum(mWorldBound.getMaximum().y, height);
			}
		}
		mSectorVerts = HardwareBufferMgr::getSingletonPtr()->createVertexBuffer(sizeof(BaseTerrain::SectorVertex), mXVerts * mZVerts, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false);
		mSectorVerts->writeData(0, mXVerts * mZVerts * sizeof(BaseTerrain::SectorVertex), (void*)pVerts);

		delete  [] pVerts;
		
		mVertexBufferBinding = HardwareBufferMgr::getSingletonPtr()->createVertexBufferBinding();
		mVertexBufferBinding->setBinding(0, mCreator->getHorzVertexData());
		mVertexBufferBinding->setBinding(1, mSectorVerts);

		mTerrainSectionRend = TITAN_NEW TerrainSectionRend(this);
		RenderData* rd = mTerrainSectionRend->getRenderData();
		_buildRenderData(rd);

		Vector4 uvScaleOffset = Vector4((float)1.0f/(mCreator->getSectorCountX()+1),
										(float)1.0f/(mCreator->getSectorCountZ()+1),
										(float)mSectorX,
										(float)mSectorZ);
		mTerrainSectionRend->setCustomShaderParam(0, uvScaleOffset);

	}
	//-------------------------------------------------------------------------------//
	void BaseTerrainSection::_buildRenderData(RenderData* rend)
	{
		rend->operationType = OT_TRIANGLE_STRIP;
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
		}
		else
			return;
	}
	//-------------------------------------------------------------------------------//




	BaseTerrainSection::TerrainSectionRend::TerrainSectionRend(BaseTerrainSection* creator)
		:mCreator(creator), mWorldPos(Matrix4::IDENTITY)
	{
		if(mCreator)
			mWorldPos.setTrans(mCreator->getSectionPos());
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
	//------------------------------------------------------------------------------//
	void BaseTerrainSection::TerrainSectionRend::setSectionPos(const Vector3& pos)
	{
		mWorldPos.setTrans(pos);
	}
	
}