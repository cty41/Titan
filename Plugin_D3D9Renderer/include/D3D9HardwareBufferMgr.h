#ifndef _D3D9_HARDWAREBUFFERMANAGER_HH
#define _D3D9_HARDWAREBUFFERMANAGER_HH

#include "D3D9Prerequisites.h"
#include "TiHardwareBufferMgr.h"

namespace Titan
{
	class _D3D9DllExport D3D9HardwareBufferMgr : public HardwareBufferMgr
	{
	public:
		D3D9HardwareBufferMgr();

		~D3D9HardwareBufferMgr();

		virtual VertexBufferSharedPtr	createVertexBuffer(
			size_t vertexSize, size_t numVertices, VertexBuffer::Usage usage, bool useSystemMemory);

		virtual IndexBufferSharedPtr	createIndexBuffer(
			size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory);

		VertexDeclaration*				createVertexDeclaration();
	};
}
#endif