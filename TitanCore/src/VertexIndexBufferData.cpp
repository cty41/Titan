#include "TitanStableHeader.h"
#include "VertexIndexBufferData.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	VertexData::VertexData(HardwareBufferManager* mgr )
	{
		mMgr = mgr ? mgr : HardwareBufferManager::getSingletonPtr();
		vertexDecl = mMgr->createVertexDeclaration();
		vertexBufferBinding = mMgr->createVertexBufferBinding();
	}
	//-------------------------------------------------------------//
	VertexData::VertexData(VertexDeclaration* decl, VertexBufferBinding* binding)
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
	{
	}
	//-------------------------------------------------------------//
	IndexData::~IndexData()
	{
	}
}