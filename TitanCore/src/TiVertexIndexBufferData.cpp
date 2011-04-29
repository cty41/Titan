#include "TitanStableHeader.h"
#include "TiVertexIndexBufferData.h"
#include "TiHardwareBufferMgr.h"

namespace Titan
{
	VertexData::VertexData(HardwareBufferMgr* mgr )
		: vertexStart(0), vertexCount(0)
	{
		mMgr = mgr ? mgr : HardwareBufferMgr::getSingletonPtr();
		vertexDecl = mMgr->createVertexDeclaration();
		vertexBufferBinding = mMgr->createVertexBufferBinding();
		mDeleteDclBinding = true;
	}
	//-------------------------------------------------------------------------------//
	VertexData::VertexData(VertexDeclaration* decl, VertexBufferBinding* binding)
		: vertexStart(0), vertexCount(0)
	{
		mMgr = HardwareBufferMgr::getSingletonPtr();
		vertexDecl = decl;
		vertexBufferBinding = binding;
		mDeleteDclBinding = false;
	}
	//-------------------------------------------------------------------------------//
	VertexData::~VertexData()
	{
		mDeleteDclBinding = false;
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