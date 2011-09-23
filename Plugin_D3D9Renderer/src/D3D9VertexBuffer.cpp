#include "D3D9VertexBuffer.h"
#include "TiException.h"
#include "D3D9Renderer.h"
#include "D3D9Mappings.h"

namespace Titan
{
	D3D9VertexBuffer::D3D9VertexBuffer(HardwareBufferMgr *mgr, size_t vertexSize, size_t numVertices, 
		VertexBuffer::Usage usage, bool useSystemMemory)
		: VertexBuffer(mgr, vertexSize, numVertices, usage,useSystemMemory),
		mBuffer(0)
	{
		D3DPOOL eResourcePool;
		eResourcePool = useSystemMemory? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;
		
		mBufferDesc.Pool = eResourcePool;

		createD3D9Buffer();
	}
	//-------------------------------------------------------------//
	D3D9VertexBuffer::~D3D9VertexBuffer()
	{
		SAFE_RELEASE(mBuffer);
	}
	//-------------------------------------------------------------//
	void D3D9VertexBuffer::createD3D9Buffer()
	{
		HRESULT hr;
		
		if(mBuffer != 0)
		{
			SAFE_RELEASE(mBuffer);
		}

		LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
		hr = pD3D9Device->CreateVertexBuffer(
			mSizeInBytes,
			D3D9Mappings::convertToD3D9(mUsage),
			0,
			mBufferDesc.Pool,
			&mBuffer,
			0);

		mLockOption = HBL_NORMAL;

		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "Cannot create D3D9 vertex buffer: " + msg);
		}

	}
	//-------------------------------------------------------------//
	void D3D9VertexBuffer::writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer /* = false */)
	{
		void* pDst = this->lock(offset, length,
			discardWholeBuffer ? HardwareBuffer::HBL_DISCARD : HardwareBuffer::HBL_NORMAL);
		memcpy(pDst, pSource, length);
		this->unlock();
	}
	//-------------------------------------------------------------//
	void D3D9VertexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		void* pSrc = this->lock(offset, length, HardwareBuffer::HBL_READ_ONLY);
		memcpy(pDest, pSrc, length);
		this->unlock();
	}
	//-------------------------------------------------------------//
	void* D3D9VertexBuffer::lockImpl(size_t offset, size_t length, LockOptions lo)
	{
		HRESULT hr;

		void* dstBytes;
		hr = mBuffer->Lock(offset, length, (void**)&dstBytes, D3D9Mappings::convertToD3D9(mLockOption, mUsage));
		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "D3D9 lock vertex buffer failed: " + msg);
		}
		return (void*)((char*)dstBytes + offset);
	}
	//-------------------------------------------------------------//
	void D3D9VertexBuffer::unlockImpl()
	{
		HRESULT hr;
		hr = mBuffer->Unlock();
		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( 
				"D3D9 unlock vertex buffer failed: " + msg);
		}
	}
}