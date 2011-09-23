#ifndef __TITAN_D3D11_VERTEXBUFFER_HH
#define __TITAN_D3D11_VERTEXBUFFER_HH

#include "D3D11Prerequisites.h"
#include "TiVertexBuffer.h"

namespace Titan
{
	class _D3D11DllExport D3D11VertexBuffer : public VertexBuffer
	{
	public:
		D3D11VertexBuffer(HardwareBufferMgr *mgr, size_t vertexSize, size_t numVertices,
			VertexBuffer::Usage usage, bool useSystemMemory);

		~D3D11VertexBuffer();

		//since d3d11 do not support offset lock so first param is useless
		void readData(size_t offset, size_t length, void* pDest);

		//since d3d11 do not support offset lock so first param is useless
		void writeData(size_t offset, size_t length, const void* pSource,
			bool discardWholeBuffer = false);

		void createD3D11Buffer();

		ID3D11Buffer*		getD3D11VertexBuffer(){ return mBuffer; }

	protected:
		void*	lockImpl(size_t offset, size_t length, LockOptions lo);

		void	unlockImpl();

	protected:
		ID3D11Buffer*		mBuffer;
		D3D11_BUFFER_DESC	mBufferDesc;
	};
}
#endif