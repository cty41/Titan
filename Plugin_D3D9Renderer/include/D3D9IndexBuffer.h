#ifndef __D3D9_INDEXBUFFER_HH
#define __D3D9_INDEXBUFFER_HH

#include "D3D9Prerequisites.h"
#include "TiIndexBuffer.h"

namespace Titan
{
	class _D3D9DllExport D3D9IndexBuffer : public IndexBuffer
	{
	public:
		D3D9IndexBuffer(HardwareBufferMgr *mgr, size_t numIndexes, 
			HardwareBuffer::Usage usage, bool useSystemMem);

		 ~D3D9IndexBuffer();

		 void readData(size_t offset, size_t length, void* pDest);

		 void writeData(size_t offset, size_t length, const void* pSource,
			 bool discardWholeBuffer = false);

		 void createD3D9Buffer();

		 IDirect3DIndexBuffer9*		getD3D9IndexBuffer() const { return mBuffer; }

	protected:
		IDirect3DIndexBuffer9*		mBuffer;
		D3DINDEXBUFFER_DESC			mBufferDesc;

	protected:
		void* lockImpl(size_t offset, size_t length, LockOptions lo);

		void unlockImpl();

	};
}
#endif