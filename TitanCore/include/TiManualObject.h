#ifndef _TITAN_MANUALOBJECT_HH
#define _TITAN_MANUALOBJECT_HH

#include "TiPrerequisites.h"
#include "TiRenderable.h"
#include "TiRenderData.h"
#include "TiVector3.h"
#include "TiVertexIndexBufferData.h"
#include "TiCommon.h"
#include "TiSceneObject.h"

#include "TitanAABB.h"
#include "TiMaterial.h"

namespace Titan
{
	class _DllExport ManualObject : public SceneObject
	{
	public:
		ManualObject(const String& name);

		~ManualObject();

		class ManualObjectSection;

		const String&	getType() const { return mType; }

		void	_updateRenderQueue(RenderQueue* queue, Camera* cam);

		void	begin(OperationType ot = OT_TRIANGLE_LIST);

		ManualObjectSection*	end();
		
		void	clear();

		void	position(const Vector3& pos);

		void	position(float x, float y, float z);

		void	texCoord(float u, float v);

		void	texCoord(const Vector2& uv);

		void	texCoord(float u, float v, float w);

		void	texCoord(const Vector3& uvw);

		void	index(uint idx);

		void	triangle(uint32 i1, uint32 i2, uint32 i3);

		void	quad(uint32 i1, uint32 i2, uint32 i3, uint32 i4);

		const AABB& getAABB() const { return mAABB;}

		void	setMaterial(const String&  name, const String&  group);

	
		struct ManualVertex
		{
			Vector3		position;
			Vector3		normal;
			Vector4		texCoords[TITAN_MAX_TEXTURE_COORD_SETS];
			ushort		texCoordDims[TITAN_MAX_TEXTURE_COORD_SETS];
			Color		color;
		};

		class _DllExport ManualObjectSection : public Renderable
		{
		protected:		
			ManualObject*	mParent;
			MaterialPtr		mMaterial;
			String			mMaterialName;
			String			mMaterialGroup;
			RenderData		mRenderData;

		public:
			ManualObjectSection(ManualObject* parent, OperationType ot);

			~ManualObjectSection();

			void			getRenderData(RenderData& rd);

			RenderData*		getRenderData() { return &mRenderData; }

			void			getTransformMat(Matrix4* transMat);

			const MaterialPtr&	getMaterial() const;

			void			setMaterial(const String&  name, const String& group);

			float			getSquaredDistance(Camera* cam);

			virtual bool			IsUseIdentityView() const {return false;}

			virtual bool			IsUseIdentityProj() const { return false;}

		};

	protected:
		String			mName;
		AABB			mAABB;

		typedef vector<ManualObjectSection*>::type	ManualObjectSectionVector;
		ManualObjectSectionVector mSectionVector;
		ManualObjectSection*	mCurrentSection;
		/// Temp storage
		ManualVertex mTempVertex;
		/// First vertex indicator
		bool mFirstVertex;
		/// Temp vertex data to copy?
		bool mTempVertexPending;
		/// System-memory buffer whilst we establish the size required
		char* mTempVertexBuffer;
		/// System memory allocation size, in bytes
		size_t mTempVertexSize;
		/// System-memory buffer whilst we establish the size required
		uint16* mTempIndexBuffer;
		/// System memory allocation size, in bytes
		size_t mTempIndexSize;
		/// Current declaration vertex size
		size_t mDeclSize;

		ushort mTexCoordIndex;

		void copyTempVertexToBuffer();

		/// Delete temp buffers and reset init counts
		void resetTempAreas(void);

		void resizeTempVertexBufferIfNeeded(size_t numVerts);

		void resizeTempIndexBufferIfNeeded(size_t numInds);
	};

	//
	//ManualObject Factory
	//
	class _DllExport ManualObjectFactory : public SceneObjectFactory
	{
	public:
		ManualObjectFactory();

		~ManualObjectFactory();

		SceneObject*	createInstance(const String& name);

		void			destroyInstance(SceneObject* object);

		const String&	getType() const { return mType; }
	protected:
		static String	mType;
	};
}
#endif