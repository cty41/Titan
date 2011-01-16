#include "D3D9HardwareBufferManager.h"
#include "D3D9VertexBuffer.h"
#include "D3D9IndexBuffer.h"
#include "D3D9VertexDeclaration.h"

namespace Titan
{
	D3D9HardwareBufferManager::D3D9HardwareBufferManager()
	{

	}
	//-------------------------------------------------------------//
	D3D9HardwareBufferManager::~D3D9HardwareBufferManager()
	{
	}
	//-------------------------------------------------------------//
	VertexBufferSharedPtr D3D9HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t numVertices, VertexBuffer::Usage usage, bool useSystemMemory)
	{
		assert(numVertices > 0);
		D3D9VertexBuffer* buf = TITAN_NEW D3D9VertexBuffer(this, vertexSize, numVertices, usage, useSystemMemory);
		mVertexBuffers.insert(buf);
		return VertexBufferSharedPtr(buf);
	}
	//-------------------------------------------------------------//
	IndexBufferSharedPtr D3D9HardwareBufferManager::createIndexBuffer(size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory)
	{
		assert(numIndexes > 0);
		D3D9IndexBuffer* buf = TITAN_NEW D3D9IndexBuffer(this, numIndexes, usage, useSystemMemory);
		mIndexBuffers.insert(buf);
		return IndexBufferSharedPtr(buf);
	}
	//-------------------------------------------------------------//
	VertexDeclaration* D3D9HardwareBufferManager::createVertexDeclaration()
	{
		D3D9VertexDeclaration* decl = TITAN_NEW D3D9VertexDeclaration();
		mVertexDeclarations.insert(decl);
		return decl;
	}
}