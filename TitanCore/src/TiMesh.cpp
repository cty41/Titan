#include "TitanStableHeader.h"
#include "TiMesh.h"
#include "TiVertexIndexBufferData.h"

namespace Titan
{
	SubMesh::SubMesh(Mesh* parent, const String& name)
		:mParent(parent), mName(name), mIndexData(nullptr), mVertexData(nullptr)
	{
	}

	SubMesh::~SubMesh()
	{
		TITAN_SAFE_DELETE(mVertexData);
		TITAN_SAFE_DELETE(mIndexData);
	}


	Mesh::Mesh(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group, bool isManual)
		:Resource(mgr, name, id, group, isManual)
	{

	}

	Mesh::~Mesh()
	{

	}

	SubMesh* Mesh::createSubMesh(const String& name)
	{
		auto it = mSubMeshHashMap.find(name);
		if(it != mSubMeshHashMap.end())
		{
			TITAN_EXCEPT_INVALIDPARAMS("Already existed a same name SubMesh " + name);
			return it->second;
		}
		SubMesh* sMesh = TITAN_NEW SubMesh(this, name);
		mSubMeshs.push_back(sMesh);
		mSubMeshHashMap.insert(SubMeshHashMap::value_type(name, sMesh));
		return sMesh;
	}

	void Mesh::removeSubMesh(const String& name)
	{
		auto it = mSubMeshHashMap.find(name);
		if(it != mSubMeshHashMap.end())
		{
			for(auto vit = mSubMeshs.begin(); vit != mSubMeshs.end();++vit)
			{
				if(*vit == it->second)
				{
					mSubMeshs.erase(vit);				
					break;
				}
			}
			TITAN_SAFE_DELETE(it->second);
			mSubMeshHashMap.erase(it);

		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS("does not exist a subMesh called this name "+ name);
		}
	}

	//super class override [begin]
	void Mesh::prepareImpl()
	{
		if (isManualLoaded())
			return;

		DataStreamPtr dataStream = ResourceGroupMgr::getSingltonPtr()->openResource(mName, mGroup);
		mPreparedData = MemoryDataStreamPtr(TITAN_NEW MemoryDataStream(dataStream));
	}

	void Mesh::unprepareImpl()
	{
		mPreparedData.setNull();
	}
	
	void Mesh::loadImpl()
	{
		//todo
	}

	void Mesh::unloadImpl()
	{
		for(auto it = mSubMeshs.begin(); it != mSubMeshs.end();++it)
			TITAN_SAFE_DELETE(*it);

		mSubMeshs.clear();
		mSubMeshHashMap.clear();
	}
	//super class override [End]


}