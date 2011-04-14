#ifndef __TITAN_BASETERRAINSECTION__HH
#define __TITAN_BASETERRAINSECTION__HH

#include "TitanPrerequisites.h"
#include "SceneObject.h"
#include "TiRenderable.h"
#include "TitanShaderEffect.h"
#include "TitanRect2D.h"
#include "VertexBuffer.h"
#include "TitanBaseTerrain.h"


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

			virtual void			getRenderData(RenderData& rd){ rd = mRenderData; }

			virtual void			getTransformMat(Matrix4* transMat){ *transMat = mCreator->_getAttachedNodeFullTransform(); }

			virtual RenderData*		getRenderData() {return &mRenderData; }

			const MaterialPtr&		getMaterial() const { return mCreator->getMaterial();}

			virtual bool			hasTexture() const { return false; }

			virtual const TexturePtr& getTexture() const ;

			virtual ShaderEffectPtr	getShaderEffect(){ return mCreator->getCreator()->getShaderEffect();}

			virtual bool			hasShader(){return (!mCreator->getCreator()->getShaderEffect().isNull());}

			virtual float			getSquaredDistance(Camera* cam);

			virtual bool			IsUseIdentityView() const {return false;}

			virtual bool			IsUseIdentityProj() const { return false;}

			virtual void			customUpdate();

			void	setCreator(BaseTerrainSection* creator){ mCreator = creator; }

			void	_buildRenderData(BaseTerrainSection* creator);

		protected:
			BaseTerrainSection*		mCreator;
			RenderData				mRenderData;

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

		uint			getPosOffsetIndex() const { return mPosOffsetIdx;}

		const Vector4&	getPosOffset() const { return mPosOffset; }

		const MaterialPtr&	getMaterial() const { return mCreator->getMaterial(); }

		//interface implement
		virtual const String&	getType() const { return mType; }

		virtual void			_updateRenderQueue(RenderQueue* queue, Camera* cam);

		virtual const AABB&		getAABB() const { return mWorldBound; }


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

		uint					mPosOffsetIdx;
		Vector4					mPosOffset;
		



	};
}

#endif