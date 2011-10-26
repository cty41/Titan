#include "TitanStableHeader.h"
#include "TiGeoTerrainSection.h"
#include "TiRenderQueue.h"

namespace Titan
{
	GeoTerrainSection::GeoTerrainSection()
		: BaseTerrainSection(), mCurrentLod(0)
	{
	}
	//------------------------------------------------------------------------------//
	GeoTerrainSection::~GeoTerrainSection()
	{

	}
	//------------------------------------------------------------------------------//
	void GeoTerrainSection::create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ, uint16 heightMapX, uint16 heightMapZ, uint16 xVerts, uint16 zVerts, const Rect2D& worldRect)
	{
		BaseTerrainSection::create(creator, sectorX, sectorZ, heightMapX, heightMapZ, xVerts, zVerts, worldRect);
		
		_calcErrorMetricsTable();
		_createConnectorRend();
	}
	//------------------------------------------------------------------------------//
	void GeoTerrainSection::_calcErrorMetricsTable()
	{
		int vertCount = 1;
		int stepCount = 8;

		for(uint lod = 0; lod < TOTAL_DETAIL_LEVELS; ++lod)
		{
			mErrorMetrics[lod] = mCreator->computeErrorMetricOfGrid(
				vertCount + 1, vertCount + 1,
				stepCount - 1, stepCount - 1,
				mHeightMapX, mHeightMapZ);

			vertCount <<= 1;
			stepCount >>= 1;
		}

		mErrorMetrics[1] = maximum(mErrorMetrics[1], mErrorMetrics[2]);
		mErrorMetrics[0] = maximum(mErrorMetrics[0], mErrorMetrics[1]);
	}
	//------------------------------------------------------------------------------//
	void GeoTerrainSection::_createConnectorRend()
	{
		for(int i = 0;i < GeoTerrain::TotalSides; ++i)
		{
			mConnectorRends[i].setCreator(this);
			mConnectorRends[i].setSectionPos(mSectionPos);
			RenderData* rend = mConnectorRends[i].getRenderData();
			rend->vertexData = TITAN_NEW VertexData(mCreator->getVertexDecl(),mVertexBufferBinding);
			rend->vertexData->vertexCount = mCreator->getHorzVertexData()->getNumVertices();

			rend->useIndex = true;
			rend->indexData = TITAN_NEW IndexData();
			rend->indexData->indexStart = 0;

			Vector4 uvScaleOffset = Vector4((float)1.0f/(mCreator->getSectorCountX()+1),
				(float)1.0f/(mCreator->getSectorCountZ()+1),
				(float)mSectorX,
				(float)mSectorZ);
			mConnectorRends[i].setCustomShaderParam(0, uvScaleOffset);
		}
	}
	//------------------------------------------------------------------------------//
	void GeoTerrainSection::_updateRenderQueue(RenderQueue* queue, Camera* cam)
	{
		if(cam->isVisible(mSectionBound))
		{
			_calcCurrentLod(cam);
			
			int index = (mSectorZ * mCreator->getSectorCountX()) + mSectorX;
			uint16 bodyTile = 0;
			uint16 sectorCountX = mCreator->getSectorCountX(), sectorCountZ = mCreator->getSectorCountZ();

			GeoTerrain* geoCreator = static_cast<GeoTerrain*>(mCreator);
			if(mCurrentLod)
			{
				//left connector
				if(mSectorX)
				{
					int sideLod = geoCreator->getGeoSection(index - 1)->getCurrentLod();
					if(sideLod < mCurrentLod)
					{
						bodyTile += 1<<GeoTerrain::Left;
						RenderData* rd = mConnectorRends[GeoTerrain::Left].getRenderData();
						GeoTerrain::IndexBufferInfo* indexInfo = geoCreator->getDetailLevelStruct(mCurrentLod).tileConnectors[GeoTerrain::Left][sideLod];
						rd->operationType = indexInfo->isStrip ? OT_TRIANGLE_STRIP: OT_TRIANGLE_LIST;
						rd->indexData->indexBuffer = indexInfo->pIndex;
						rd->indexData->indexCount = rd->indexData->indexBuffer->getNumIndexes();
						queue->addRenderable(&mConnectorRends[GeoTerrain::Left]);
					}
				}
				//top connector
				if(mSectorZ)
				{
					int sideLod = geoCreator->getGeoSection(index - sectorCountX)->getCurrentLod();
					if(sideLod < mCurrentLod)
					{
						bodyTile += 1<<GeoTerrain::Top;
						RenderData* rd = mConnectorRends[GeoTerrain::Top].getRenderData();
						GeoTerrain::IndexBufferInfo* indexInfo = geoCreator->getDetailLevelStruct(mCurrentLod).tileConnectors[GeoTerrain::Top][sideLod];
						rd->operationType = indexInfo->isStrip ? OT_TRIANGLE_STRIP: OT_TRIANGLE_LIST;
						rd->indexData->indexBuffer = indexInfo->pIndex;
						rd->indexData->indexCount = rd->indexData->indexBuffer->getNumIndexes();
						queue->addRenderable(&mConnectorRends[GeoTerrain::Top]);
					}
				}
				//right connector
				if(mSectorX < sectorCountX - 1)
				{
					int sideLod = geoCreator->getGeoSection(index + 1)->getCurrentLod();
					if(sideLod < mCurrentLod)
					{
						bodyTile += 1<<GeoTerrain::Right;
						RenderData* rd = mConnectorRends[GeoTerrain::Right].getRenderData();
						GeoTerrain::IndexBufferInfo* indexInfo = geoCreator->getDetailLevelStruct(mCurrentLod).tileConnectors[GeoTerrain::Right][sideLod];
						rd->operationType = indexInfo->isStrip ? OT_TRIANGLE_STRIP: OT_TRIANGLE_LIST;
						rd->indexData->indexBuffer = indexInfo->pIndex;
						rd->indexData->indexCount = rd->indexData->indexBuffer->getNumIndexes();
						queue->addRenderable(&mConnectorRends[GeoTerrain::Right]);
					}
				}
				//bottom connector
				if(mSectorZ < sectorCountZ - 1)
				{
					int sideLod = geoCreator->getGeoSection(index + sectorCountX)->getCurrentLod();
					if(sideLod < mCurrentLod)
					{
						bodyTile += 1<<GeoTerrain::Bottom;
						RenderData* rd = mConnectorRends[GeoTerrain::Bottom].getRenderData();
						GeoTerrain::IndexBufferInfo* indexInfo = geoCreator->getDetailLevelStruct(mCurrentLod).tileConnectors[GeoTerrain::Bottom][sideLod];
						rd->operationType = indexInfo->isStrip ? OT_TRIANGLE_STRIP: OT_TRIANGLE_LIST;
						rd->indexData->indexBuffer = indexInfo->pIndex;
						rd->indexData->indexCount = rd->indexData->indexBuffer->getNumIndexes();
						queue->addRenderable(&mConnectorRends[GeoTerrain::Bottom]);
					}
				}
			}

			//lod 1 and tile 15 does not need to be drawn
			if(mCurrentLod != 1 || bodyTile != 15)
			{
				RenderData* rend = mTerrainSectionRend->getRenderData();
				GeoTerrain::IndexBufferInfo* indexInfo = geoCreator->getDetailLevelStruct(mCurrentLod).tileBodies[bodyTile];
				rend->operationType = indexInfo->isStrip ? OT_TRIANGLE_STRIP: OT_TRIANGLE_LIST;
				rend->indexData->indexBuffer = indexInfo->pIndex;
				rend->indexData->indexCount = rend->indexData->indexBuffer->getNumIndexes();
				queue->addRenderable(mTerrainSectionRend);
			}
		}
	}
	//------------------------------------------------------------------------------//
	void GeoTerrainSection::_calcCurrentLod(Camera* cam)
	{
		Vector2 corner0(mSectionBound.getMinimum().x, mSectionBound.getMinimum().z);
		Vector2 corner1(mSectionBound.getMinimum().x, mSectionBound.getMaximum().z);
		Vector2 corner2(mSectionBound.getMaximum().x, mSectionBound.getMaximum().z);
		Vector2 corner3(mSectionBound.getMaximum().x, mSectionBound.getMinimum().z);

		Vector2 viewPoint(cam->getPosition().x, cam->getPosition().z);

		// compute view distance to our 4 corners
		float distance0 = viewPoint.distance(corner0);
		float distance1 = viewPoint.distance(corner1);
		float distance2 = viewPoint.distance(corner2);
		float distance3 = viewPoint.distance(corner3);

		// compute min distance as the test value
		float dist = minimum(distance0, distance1);
		dist = minimum(dist, distance2);
		dist = minimum(dist, distance3);

		// make sure the minimum distance is non-zero
		dist = maximum(dist, 0.0001f);

		// find the lowest lod which will suffice
		mCurrentLod = 0;
		bool finished = false;

		float fScale = mCreator->getLodErrorScale();
		float fLimit = mCreator->getRatioLimit();

		while (!finished)
		{
			// find the ratio of variance over distance
			float variance = mErrorMetrics[mCurrentLod];
			float vRatio = (variance * fScale) / dist;

			// if we exceed the ratio limit, move to the next lod
			if (vRatio > fLimit
				&& mCurrentLod + 1 < TOTAL_DETAIL_LEVELS)
			{
				++mCurrentLod;
			}
			else
			{
				finished=true;
			}
		}
	}
}