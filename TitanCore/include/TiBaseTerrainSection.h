#ifndef __TITAN_BASETERRAINSECTION__HH
#define __TITAN_BASETERRAINSECTION__HH

#include "TiPrerequisites.h"
#include "TiSceneObject.h"
#include "TiRenderable.h"

#include "TiRect2D.h"
#include "TiVertexBuffer.h"
#include "TiBaseTerrain.h"


namespace Titan
{
	class _DllExport BaseTerrainSection : public SceneObject
	{
	public:
		class _DllExport TerrainSectionRend : public Renderable
		{
		public:
			TerrainSectionRend(BaseTerrainSection* creator = NULL);

			~TerrainSectionRend();

			void getRenderData(RenderData& rd){ rd = mRenderData; }

			void getTransformMat(Matrix4* transMat){ *transMat = mWorldPos; }

			RenderData*	getRenderData() {return &mRenderData; }

			const MaterialPtr&		getMaterial() const { return mCreator->getMaterial();}

			virtual float			getSquaredDistance(Camera* cam);

			virtual bool			IsUseIdentityView() const {return false;}

			virtual bool			IsUseIdentityProj() const { return false;}

			void	setCreator(BaseTerrainSection* creator){ mCreator = creator; }

			void	_buildRenderData(BaseTerrainSection* creator);

			void	setSectionPos(const Vector3& pos);

		protected:
			BaseTerrainSection*		mCreator;
			RenderData				mRenderData;
			Matrix4					mWorldPos;

		};

		typedef vector<TerrainSectionRend*>::type TerrainSectionRendVec;
	public:
		BaseTerrainSection();

		virtual ~BaseTerrainSection();

		virtual void create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ,
			uint16 heightMapX, uint16 heightMapZ,
			uint16 xVerts, uint16 zVerts, 
			const Rect2D& worldRect);

		virtual uint16 getSectorX() const { return mSectorX; }

		virtual uint16 getSectorZ() const { return mSectorZ; }

		BaseTerrain*	getCreator() const { return mCreator; }

		VertexBufferSharedPtr	getSectorVertices() { return mSectorVerts; }

		const AABB&		getWorldBound() const { return mWorldBound; }

		const MaterialPtr&	getMaterial() const { return mCreator->getMaterial(); }

		//interface implement
		virtual const String&	getType() const { return mType; }

		virtual void			_updateRenderQueue(RenderQueue* queue, Camera* cam);

		virtual const AABB&		getAABB() const { return mWorldBound; }

		const Vector3&	getSectionPos() const { return mSectionPos; }


	protected:

		virtual void		_buildVertexBuffer();

		virtual void		_buildRenderData(RenderData* rend);

	protected:
		BaseTerrain*			mCreator;
		VertexBufferSharedPtr	mSectorVerts;
		VertexBufferBinding*	mVertexBufferBinding;

		TerrainSectionRend*		mTerrainSectionRend;
		TerrainSectionRendVec	mTerrainSectionRendVec;
		AABB					mWorldBound;
		
		uint16					mHeightMapX;
		uint16					mHeightMapZ;
		uint16					mSectorX; 
		uint16					mSectorZ; 
		uint16					mXVerts; 
		uint16					mZVerts;

		Vector3					mSectionPos;
	};
}

#endif