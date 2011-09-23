#ifndef __TITAN_D3D11_INDEXBUFFER_HH
#define __TITAN_D3D11_INDEXBUFFER_HH

#include "D3D11Prerequisites.h"
#include "TiIndexBuffer.h"

namespace Titan
{
	class _D3D11DllExport D3D11IndexBuffer : public IndexBuffer
	{
	public:
		D3D11IndexBuffer(HardwareBufferMgr *mgr, size_t numIndexes, 
			HardwareBuffer::Usage usage, bool useSystemMem);

		~D3D11IndexBuffer();

		void readData(size_t offset, size_t length, void* pDest);

		void writeData(size_t offset, size_t length, const void* pSource,
			bool discardWholeBuffer = false);

		void createD3D11Buffer();

		ID3D11Buffer*		getD3D11IndexBuffer() const { return mBuffer; }

	protected:
		ID3D11Buffer*		mBuffer;
		D3D11_BUFFER_DESC	mBufferDesc;

	protected:
		void* lockImpl(size_t offset, size_t length, LockOptions lo);

		void unlockImpl();
	};
}
#endif