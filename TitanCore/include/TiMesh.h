#ifndef _TTIAN__MESH__HH
#define _TTIAN__MESH__HH

#include "TiPrerequisites.h"
#include "TiResource.h"
#include "TitanAABB.h"
#include "TiRenderData.h"
#include "TiMaterial.h"

namespace Titan
{

	class _DllExport SubMesh: public GeneralAlloc
	{
	public:
		SubMesh(Mesh* parent, const String& name);

		~SubMesh();

		String		mName;
		Mesh*		mParent;
		MaterialPtr	mMaterial;

		OperationType	mPrimitiveType;
		IndexData*	mIndexData;
		VertexData*	mVertexData;
	};

	typedef vector<SubMesh*>::type SubMeshVec;
	typedef hash_map<String, SubMesh*>::type SubMeshHashMap;

	class _DllExport Mesh : public Resource
	{
	public:
		Mesh(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual);

		~Mesh();

		SubMesh* createSubMesh(const String& name);

		void	removeSubMesh(const String& name);

	protected:
		//super class override [begin]
		void prepareImpl();

		void unprepareImpl();

		void loadImpl();

		void unloadImpl();
		//super class override [End]



	protected:
		SubMeshVec	mSubMeshs;
		//fast name lookup
		SubMeshHashMap	mSubMeshHashMap;

		//general bounding box and sphere
		AABB	mAABB;
		float	mSphereRadius;

	};

	class _DllExport MeshPtr : public SharedPtr<Mesh>
	{
	public:
		MeshPtr()
			: SharedPtr<Mesh>()
		{}

		explicit MeshPtr(Mesh* mesh, SharedPtrFreeMethod freeMethod = SPFM_DELETE)
			: SharedPtr<Mesh>(mesh, freeMethod){}

		MeshPtr(const MeshPtr& r) : SharedPtr<Mesh>(r) {} 
		MeshPtr(const ResourcePtr& r) : SharedPtr<Mesh>()
		{
			pRep = static_cast<Mesh*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}

		}

		/// Operator used to convert a ResourcePtr to a MeshPtr
		MeshPtr& operator=(const ResourcePtr& r)
		{
			if (pRep == static_cast<Mesh*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<Mesh*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}

			return *this;
		}

	};

}
#endif