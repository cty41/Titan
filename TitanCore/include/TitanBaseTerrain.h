#ifndef __TITAN_BASE_TERRAIN__HH
#define __TITAN_BASE_TERRAIN__HH

#include "TitanPrerequisites.h"
#include "TitanTexture.h"
#include "TitanAABB.h"
#include "TitanVector3.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

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

		~BaseTerrain();

		virtual void	create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift = 3);

		virtual void	destroy();

		void			setEffect(ShaderEffectPtr effect){ mEffect = effect; };
		
		ShaderEffectPtr	getShaderEffect() { return mEffect;}

		VertexDeclaration*	getVertexDecl() const { return mVertexDecl; }

		VertexBufferSharedPtr	getHorzVertexData() const { return mHorzVertexData; }

		IndexBufferSharedPtr	getSharedIndexData() const { return mIndexData; }

		float readWorldHeight(uint16 mapX, uint16 mapZ) const;

		float readWorldHeight(uint16 mapIndex) const;

		const Vector3& readWorldNormal(uint16 mapX, uint16 mapZ) const;

		const Vector2& getSectorSize() const { return mSectorSize;}

		const AABB& getWorldBound() const { return mWorldBound; }

		
	protected:
		void			_buildHeightAndNormalTables(TexturePtr heightMap);


		virtual void	_buildHorzVertexData();

		virtual void	_buildVertexDecl();

		virtual void	_buildIndexData();

		virtual void	_createTerrainSections();

	protected:
		BaseTerrainSection*		mSectorArray;
		SceneNode*				mRootNode;

		float*					mHeightTable;
		Vector3*				mNormalTable;

		VertexBufferSharedPtr	mHorzVertexData;
		VertexDeclaration*		mVertexDecl;
		IndexBufferSharedPtr	mIndexData;

		ShaderEffectPtr		mEffect;

		AABB			mWorldBound;
		
		Vector3			mWorldSize;
		Vector3			mMapScale;
		Vector2			mSectorSize;
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