#ifndef __TITAN_BASE_TERRAIN__HH
#define __TITAN_BASE_TERRAIN__HH

#include "TiPrerequisites.h"
#include "TiTexture.h"
#include "TitanAABB.h"
#include "TiVector3.h"
#include "TiVertexBuffer.h"
#include "TiIndexBuffer.h"
#include "TiMaterial.h"

namespace Titan
{
	class _DllExport BaseTerrain : public GeneralAlloc
	{
	public:
		struct LocalVertex
		{
			Vector2		xzPos;
			Vector2		localUV;
		};

		struct SectorVertex
		{
			float	height;
		//	Vector3	normal;
		};


	public:
		BaseTerrain();

		virtual ~BaseTerrain();

		virtual void	create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift = 3);

		virtual void	destroy();

		VertexDeclaration*	getVertexDecl() const { return mVertexDecl; }

		VertexBufferSharedPtr	getHorzVertexData() const { return mHorzVertexData; }

		IndexBufferSharedPtr	getSharedIndexData() const { return mIndexData; }

		float readWorldHeight(uint16 mapX, uint16 mapZ) const;

		float readWorldHeight(uint16 mapIndex) const;

		const Vector3& readWorldNormal(uint16 mapX, uint16 mapZ) const;

		const Vector2& getSectorSize() const { return mSectorSize;}

		const AABB& getWorldBound() const { return mWorldBound; }

		uint8 getSectorShift() const { return mSectorShift; }

		float computeErrorMetricOfGrid(
			uint16 xVerts,	// width of grid
			uint16 zVerts,	// height of grid
			uint16 xStep,	// horz vertex count per cell
			uint16 zStep,	// vert vertex count per cell
			uint16 xOffset,	// starting index X
			uint16 zOffset);// starting index Z

		float getLodErrorScale() const { return mLodScale;}

		float getRatioLimit() const { return mRatioLimit; }

		void setTessellationParams(float lodScale, float ratioLimit){ mLodScale = lodScale; mRatioLimit = ratioLimit;}

		uint8	getSectorVertex() const { return mSectorVerts; }

		const MaterialPtr&	getMaterial() const;

		
	protected:
		void			_buildHeightAndNormalTables(TexturePtr heightMap);


		virtual void	_buildHorzVertexData();

		virtual void	_buildVertexDecl();

		virtual void	_buildIndexData();

		virtual void	_createTerrainSections();

	protected:
		BaseTerrainSection**		mSectorArray;
		SceneNode*				mRootNode;

		float*					mHeightTable;
		Vector3*				mNormalTable;

		VertexBufferSharedPtr	mHorzVertexData;
		VertexDeclaration*		mVertexDecl;
		IndexBufferSharedPtr	mIndexData;

		AABB			mWorldBound;
		
		Vector3			mWorldSize;
		Vector3			mMapScale;
		Vector2			mSectorSize;
		float			mLodScale;
		float			mRatioLimit;
		uint16			mTableWidth;
		uint16			mTableHeight;
		uint16			mSectorCountX;
		uint16			mSectorCountZ;
		uint8			mSectorShift;
		uint8			mSectorUnits;
		uint8			mSectorVerts;
	};
}


#endif