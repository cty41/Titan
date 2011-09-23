#ifndef __TITAN_D3D11HARDWAREBUFFERMGR__HH
#define __TITAN_D3D11HARDWAREBUFFERMGR__HH

#include "D3D11Prerequisites.h"
#include "TiHardwareBufferMgr.h"

namespace Titan
{
	class _D3D11DllExport D3D11HardwareBufferMgr : public HardwareBufferMgr
	{
	public:
		D3D11HardwareBufferMgr();

		~D3D11HardwareBufferMgr();

		virtual VertexBufferPtr	createVertexBuffer(
			size_t vertexSize, size_t numVertices, VertexBuffer::Usage usage, bool useSystemMemory);

		virtual IndexBufferPtr	createIndexBuffer(
			size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory);

		VertexDeclaration*		createVertexDeclaration();
	};
}
#endif