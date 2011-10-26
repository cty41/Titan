#include "D3D11VertexBuffer.h"
#include "D3D11Mappings.h"
#include "D3D11Renderer.h"

namespace Titan
{
	D3D11VertexBuffer::D3D11VertexBuffer(HardwareBufferMgr *mgr, size_t vertexSize, size_t numVertices, VertexBuffer::Usage usage, bool useSystemMemory)
		:VertexBuffer(mgr, vertexSize, numVertices, usage, useSystemMemory), mBuffer(nullptr)
	{
		memset(&mBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		mBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		mBufferDesc.ByteWidth = mSizeInBytes;
		mBufferDesc.Usage = D3D11Mappings::convertToD3D11(usage);
		mBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		createD3D11Buffer();
	}

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{
		SAFE_RELEASE(mBuffer);
	}

	void D3D11VertexBuffer::createD3D11Buffer()
	{
		HRESULT hr;
		ID3D11Device* pd3dDevice =  D3D11Renderer::getSingletonPtr()->__getD3D11Device();
		hr = pd3dDevice->CreateBuffer(&mBufferDesc, 0, &mBuffer);
		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "Create d3d11 vertex buffer failed: ");
		}
	}

	void* D3D11VertexBuffer::lockImpl(size_t offset, size_t length, LockOptions lo)
	{
		HRESULT hr;
		void* dstBytes;
		ID3D11DeviceContext* pDeviceContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
		D3D11_MAPPED_SUBRESOURCE mappedSubRes;
		mappedSubRes.pData = nullptr;

		hr = pDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0 , &mappedSubRes);
		if(FAILED(hr))
		{
			TITAN_EXCEPT_API_D11(hr, "D3D11 Map vertex buffer failed: ");
		}
		dstBytes = (void*)mappedSubRes.pData;
		return dstBytes;
	}

	void D3D11VertexBuffer::unlockImpl()
	{
		ID3D11DeviceContext* pDeviceContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
		pDeviceContext->Unmap(mBuffer, 0);
	}

	void D3D11VertexBuffer::writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer /* = false */)
	{
		void* pDst = this->lock(offset, length,
			discardWholeBuffer ? HardwareBuffer::HBL_DISCARD : HardwareBuffer::HBL_NORMAL);
		memcpy(pDst, pSource, length);
		this->unlock();
	}
	//-------------------------------------------------------------//
	void D3D11VertexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		void* pSrc = this->lock(offset, length, HardwareBuffer::HBL_READ_ONLY);
		memcpy(pDest, pSrc, length);
		this->unlock();
	}
}