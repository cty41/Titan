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
	}
	//-------------------------------------------------------------//
	VertexData::VertexData(VertexDeclaration* decl, VertexBufferBinding* binding)
		: vertexStart(0), vertexCount(0)
	{
		mMgr = HardwareBufferManager::getSingletonPtr();
		vertexDecl = decl;
		vertexBufferBinding = binding;
	}
	//-------------------------------------------------------------//
	VertexData::~VertexData()
	{
		mMgr->destroyVertexBufferBinding(vertexBufferBinding);
		mMgr->destroyVertexDeclaration(vertexDecl);
	}
	


	//-------------------------------------------------------------//
	IndexData::IndexData()
		: indexStart(0), indexCount(0)
	{
	}
	//-------------------------------------------------------------//
	IndexData::~IndexData()
	{
	}
}