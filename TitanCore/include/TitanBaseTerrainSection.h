#ifndef __TITAN_BASETERRAINSECTION__HH
#define __TITAN_BASETERRAINSECTION__HH

#include "TitanPrerequisites.h"
#include "SceneObject.h"
#include "Renderable.h"
#include "TitanShaderEffect.h"
#include "TitanRect2D.h"
#include "VertexBuffer.h"


namespace Titan
{
	class _DllExport BaseTerrainSection : public SceneObject, public Renderable
	{
	public:
		BaseTerrainSection();

		~BaseTerrainSection();

		virtual void create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ,
			uint16 heightMapX, uint16 heightMapZ,
			uint16 xVerts, uint16 zVerts, 
			const Rect2D& worldRect);

		virtual void destroy();

		virtual uint16 getSectorX() const { return mSectorX; }

		virtual uint16 getSectorZ() const { return mSectorZ; }

		BaseTerrain*	getCreator() const { return mCreator; }

		VertexBufferSharedPtr	getSectorVertices() { return mSectorVerts; }

		//interface implement
		virtual const String&	getType() const { return mType; }

		//temp method, which will be removed when we add render queue
		virtual void			_updateRenderableList(SceneMgr::RenderableList& renderableList, Camera* cam);

		virtual const AABB&		getAABB() const { return mWorldBound; }

		virtual void			getRenderData(RenderData& rd){ rd = mRenderData; }

		virtual void			getTransformMat(Matrix4* transMat);

		virtual RenderData*		getRenderData() {return &mRenderData; }

		virtual ShaderEffectPtr	getShaderEffect();

		virtual bool			hasShader();

		virtual void			customUpdate();



	protected:

		void		_buildVertexBuffer();

		void		_buildRenderData();

	protected:
		BaseTerrain*			mCreator;
		VertexBufferSharedPtr	mSectorVerts;
		RenderData				mRenderData;
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