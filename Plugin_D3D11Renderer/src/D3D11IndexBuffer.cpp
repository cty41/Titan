#include "D3D11IndexBuffer.h"
#include "D3D11Mappings.h"
#include "D3D11Renderer.h"

namespace Titan
{
	D3D11IndexBuffer::D3D11IndexBuffer(HardwareBufferMgr *mgr, size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMem)
		:IndexBuffer(mgr, numIndexes, usage, useSystemMem), mBuffer(nullptr)
	{
		memset(&mBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		mBufferDesc.ByteWidth = mSizeInBytes;
		mBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		mBufferDesc.Usage = D3D11Mappings::convertToD3D11(usage);
		mBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		createD3D11Buffer();
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
		SAFE_RELEASE(mBuffer);
	}

	void D3D11IndexBuffer::createD3D11Buffer()
	{
		HRESULT hr;
		ID3D11Device* pd3dDevice =  D3D11Renderer::getSingletonPtr()->__getD3D11Device();
		hr = pd3dDevice->CreateBuffer(&mBufferDesc, 0, &mBuffer);
		if(FAILED(hr))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("Create d3d11 index buffer failed: " + errMsg);
		}
	}
	void* D3D11IndexBuffer::lockImpl(size_t offset, size_t length, LockOptions lo)
	{
		HRESULT hr;
		void* dstBytes;
		ID3D11DeviceContext* pDeviceContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
		D3D11_MAPPED_SUBRESOURCE mappedSubRes;
		mappedSubRes.pData = nullptr;

		hr = pDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0 , &mappedSubRes);
		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "D3D11 Map index buffer failed: " + msg);
		}
		dstBytes = (void*)mappedSubRes.pData;
		return (dstBytes);
	}

	void D3D11IndexBuffer::unlockImpl()
	{
		ID3D11DeviceContext* pDeviceContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
		pDeviceContext->Unmap(mBuffer, 0);
	}

	void D3D11IndexBuffer::writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer /* = false */)
	{
		void* pDst = this->lock(offset, length,
			discardWholeBuffer ? HardwareBuffer::HBL_DISCARD : HardwareBuffer::HBL_NORMAL);
		memcpy(pDst, pSource, length);
		this->unlock();
	}
	//-------------------------------------------------------------//
	void D3D11IndexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		void* pSrc = this->lock(offset, length, HardwareBuffer::HBL_READ_ONLY);
		memcpy(pDest, pSrc, length);
		this->unlock();
	}
}