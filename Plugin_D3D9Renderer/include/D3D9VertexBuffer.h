#ifndef _TITAN_D3D9VERTEXBUFFER_HH
#define _TITAN_D3D9VERTEXBUFFER_HH

#include "D3D9Prerequisites.h"
#include "TiVertexBuffer.h"


namespace Titan
{
	class _D3D9DllExport D3D9VertexBuffer : public VertexBuffer
	{
	public:
		D3D9VertexBuffer(HardwareBufferMgr *mgr, size_t vertexSize, size_t numVertices,
			VertexBuffer::Usage usage, bool useSystemMemory);

		~D3D9VertexBuffer();

		void readData(size_t offset, size_t length, void* pDest);

		void writeData(size_t offset, size_t length, const void* pSource,
			bool discardWholeBuffer = false);

		void createD3D9Buffer();

		IDirect3DVertexBuffer9*		getD3D9VertexBuffer(){ return mBuffer; }

	protected:
		void*	lockImpl(size_t offset, size_t length, LockOptions lo);

		void	unlockImpl();

	protected:
		IDirect3DVertexBuffer9*		mBuffer;
		D3DVERTEXBUFFER_DESC		mBufferDesc;
	};
}
#endif