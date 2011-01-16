#include "D3D9IndexBuffer.h"
#include "D3D9Mappings.h"
#include "Exception.h"
#include "D3D9Renderer.h"


namespace Titan
{
	D3D9IndexBuffer::D3D9IndexBuffer(HardwareBufferManager *mgr, size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMem)
		: IndexBuffer(mgr, numIndexes, usage, useSystemMem), mBuffer(0)
	{
		D3DPOOL eResourcePool;
		eResourcePool = useSystemMem? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;

		mBufferDesc.Pool = eResourcePool;

		createD3D9Buffer();
	}
	//-------------------------------------------------------------//
	D3D9IndexBuffer::~D3D9IndexBuffer()
	{
		SAFE_RELEASE(mBuffer);
	}
	//-------------------------------------------------------------//
	void D3D9IndexBuffer::createD3D9Buffer()
	{
		HRESULT hr ;
		if(mBuffer)
		{
			SAFE_RELEASE(mBuffer);
		}
		else
		{
			LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
			 hr = pD3D9Device->CreateIndexBuffer(
				static_cast<UINT>(mSizeInBytes),
				D3D9Mappings::convertToD3D9(mUsage),
				(mIndexSize == 16)? D3DFMT_INDEX16 : D3DFMT_INDEX32,
				mBufferDesc.Pool,
				&mBuffer,
				0);

				if(FAILED(hr))
				{
					String errorMsg = DXGetErrorDescription(hr);
					TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, "Create D3D9 Index Buffer failed : " + errorMsg, "D3D9IndexBuffer::createD3D9Buffer()");
				}
		}
	}
	//-------------------------------------------------------------//
	void D3D9IndexBuffer::writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer /* = false */)
	{
		void* pDst = this->lock(offset, length,
			discardWholeBuffer ? HardwareBuffer::HBL_DISCARD : HardwareBuffer::HBL_NORMAL);
		memcpy(pDst, pSource, length);
		this->unlock();
	}
	//-------------------------------------------------------------//
	void D3D9IndexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		void* pSrc = this->lock(offset, length, HardwareBuffer::HBL_READ_ONLY);
		memcpy(pDest, pSrc, length);
		this->unlock();
	}
	//-------------------------------------------------------------//
	void* D3D9IndexBuffer::lockImpl(size_t offset, size_t length, LockOptions lo)
	{
		HRESULT hr;

		void* pDst;
		hr = mBuffer->Lock(offset, length, (void**)&pDst, D3D9Mappings::convertToD3D9(mLockOption, mUsage));
		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
				"D3D9 lock index buffer failed: " + msg, 
				"D3D9IndexBuffer::lockImpl");
		}
		return (void*)((char*)pDst + offset);
	}
	//-------------------------------------------------------------//
	void D3D9IndexBuffer::unlockImpl()
	{
		HRESULT hr;
		hr = mBuffer->Unlock();
		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
				"D3D9 unlock index buffer failed: " + msg,
				"D3D9IndexBuffer::unlockImpl");
		}
	}

}
