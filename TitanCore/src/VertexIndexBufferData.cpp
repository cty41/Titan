#include "TitanStableHeader.h"
#include "VertexIndexBufferData.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	VertexData::VertexData(HardwareBufferManager* mgr )
		: vertexStart(0), vertexCount(0)
	{
		mMgr = mgr ? mgr : HardwareBufferManager::getSingletonPtr();
		vertexDecl = mMgr->createVertexDeclaration();
		vertexBufferBinding = mMgr->createVertexBufferBinding();
		mDeleteDclBinding = true;
	}
	//-------------------------------------------------------------------------------//
	VertexData::VertexData(VertexDeclaration* decl, VertexBufferBinding* binding)
		: vertexStart(0), vertexCount(0)
	{
		mMgr = HardwareBufferManager::getSingletonPtr();
		vertexDecl = decl;
		vertexBufferBinding = binding;
		mDeleteDclBinding = false;
	}
	//-------------------------------------------------------------------------------//
	VertexData::~VertexData()
	{
		if (mDeleteDclBinding)
		{
			mMgr->destroyVertexBufferBinding(vertexBufferBinding);
			mMgr->destroyVertexDeclaration(vertexDecl);
		}
	}
	


	//-------------------------------------------------------------------------------//
	IndexData::IndexData()
		: indexStart(0), indexCount(0)
	{
	}
	//-------------------------------------------------------------------------------//
	IndexData::~IndexData()
	{
	}
}