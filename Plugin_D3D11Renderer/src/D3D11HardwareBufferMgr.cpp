#include "D3D11HardwareBufferMgr.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "D3D11VertexDecl.h"

namespace Titan
{
	D3D11HardwareBufferMgr::D3D11HardwareBufferMgr()
	{

	}
	//-------------------------------------------------------------------------------//
	D3D11HardwareBufferMgr::~D3D11HardwareBufferMgr()
	{
	}
	//-------------------------------------------------------------------------------//
	VertexBufferPtr D3D11HardwareBufferMgr::createVertexBuffer(size_t vertexSize, size_t numVertices, VertexBuffer::Usage usage, bool useSystemMemory)
	{
		assert(numVertices > 0);
		D3D11VertexBuffer* buf = TITAN_NEW D3D11VertexBuffer(this, vertexSize, numVertices, usage, useSystemMemory);
		mVertexBuffers.insert(buf);
		return VertexBufferPtr(buf);
	}
	//-------------------------------------------------------------------------------//
	IndexBufferPtr D3D11HardwareBufferMgr::createIndexBuffer(size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory)
	{
		assert(numIndexes > 0);
		D3D11IndexBuffer* buf = TITAN_NEW D3D11IndexBuffer(this, numIndexes, usage, useSystemMemory);
		mIndexBuffers.insert(buf);
		return IndexBufferPtr(buf);
	}
	//-------------------------------------------------------------------------------//
	VertexDeclaration* D3D11HardwareBufferMgr::createVertexDeclaration()
	{
		D3D11VertexDecl* decl = TITAN_NEW D3D11VertexDecl();
		mVertexDeclarations.insert(decl);
		return decl;
	}
}