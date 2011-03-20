#ifndef _TITAN_MANUALOBJECT_HH
#define _TITAN_MANUALOBJECT_HH

#include "TitanPrerequisites.h"
#include "Renderable.h"
#include "RenderData.h"
#include "TitanVector3.h"
#include "VertexIndexBufferData.h"
#include "TitanCommon.h"
#include "SceneObject.h"
#include "TitanShaderEffect.h"
#include "TitanAABB.h"

namespace Titan
{
	class _DllExport ManualObject : public SceneObject
	{
	public:
		ManualObject(const String& name);

		~ManualObject();

		class ManualObjectSection;

		const String&	getType() const { return mType; }

		void	_updateRenderableList(SceneMgr::RenderableList& renderableList);

		void	begin(RenderData::OperationType ot = RenderData::OT_TRIANGLE_LIST);

		ManualObjectSection*	end();
		
		void	clear();

		void	position(const Vector3& pos);

		void	position(float x, float y, float z);

		void	index(uint idx);

		void	triangle(uint32 i1, uint32 i2, uint32 i3);

		void	quad(uint32 i1, uint32 i2, uint32 i3, uint32 i4);

		void	setShaderEffect(ShaderEffectPtr effect);

		const AABB& getAABB() const { return mAABB;}

	
		struct ManualVertex
		{
			Vector3		position;
		};

		class _DllExport ManualObjectSection : public Renderable
		{
		protected:		
			ManualObject*	mParent;
			RenderData		mRenderData;
			ShaderEffectPtr	mShaderEffect;

		public:
			ManualObjectSection(ManualObject* parent, RenderData::OperationType ot);

			~ManualObjectSection();

			void			getRenderData(RenderData& rd);

			RenderData*		getRenderData() { return &mRenderData; }

			void			getTransformMat(Matrix4* transMat);

			bool			hasShader(){ return !mShaderEffect.isNull(); }

			ShaderEffectPtr getShaderEffect() { return mShaderEffect; }

			void			setShaderEffect(ShaderEffectPtr effect){ mShaderEffect = effect; }

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
		uint32* mTempIndexBuffer;
		/// System memory allocation size, in bytes
		size_t mTempIndexSize;
		/// Current declaration vertex size
		size_t mDeclSize;

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