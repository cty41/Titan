#include "D3D11VertexDecl.h"
#include "D3D11HLSLShader.h"
#include "D3D11Mappings.h"
#include "D3D11Renderer.h"

namespace Titan
{
	D3D11VertexDecl::D3D11VertexDecl()
		:VertexDeclaration(), mNeedRecreate(true), mInputElementDescs(nullptr)
	{
	}

	D3D11VertexDecl::~D3D11VertexDecl()
	{
		for(auto itr = mShaderToLayoutMap.begin(); itr != mShaderToLayoutMap.end(); ++itr)
		{
			SAFE_RELEASE(itr->second);
		}
		mShaderToLayoutMap.clear();

		
	}

	ID3D11InputLayout* D3D11VertexDecl::getD11InputLayoutByShader(D3D11HLSLShader* shader)
	{
		 auto itr = mShaderToLayoutMap.find(shader);
		 if (itr != mShaderToLayoutMap.end())
		 {
			 return itr->second;
		 }
		 else
		 {
			 //todo
			 createInputElementDescs();

			ID3D11InputLayout* inputLayout = nullptr;
			HRESULT hr;
			ID3D11Device*	pd3dDevice = D3D11Renderer::getSingletonPtr()->__getD3D11Device();
			hr = pd3dDevice->CreateInputLayout(mInputElementDescs, (UINT)mElementList.size(), 
				shader->getCompiledCode()->GetBufferPointer(),
				shader->getCompiledCode()->GetBufferSize(),
				&inputLayout);
			if(FAILED(hr))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT_API("D3DX11 create InputLayout failed: " + errMsg);
				return nullptr;
			}

			mShaderToLayoutMap[shader] = inputLayout;
			return inputLayout;
		 }
	}

	void D3D11VertexDecl::createInputElementDescs()
	{
		if(mNeedRecreate)
		{
			if(mInputElementDescs)
				TITAN_DELETE_ARRAY_T(mInputElementDescs, D3D11_INPUT_ELEMENT_DESC, mElementList.size(), MEMCATEGORY_GENERAL)
			
			mInputElementDescs = TITAN_NEW_ARRAY_T(D3D11_INPUT_ELEMENT_DESC, mElementList.size(), MEMCATEGORY_GENERAL);
			uint idx = 0;
			for(auto itr = mElementList.begin(); itr != mElementList.end(); ++itr, ++idx)
			{
					mInputElementDescs[idx].SemanticName = D3D11Mappings::convertToD3D11(itr->getSemantic());
					mInputElementDescs[idx].SemanticIndex = itr->getIndex();
					mInputElementDescs[idx].Format = D3D11Mappings::convertToD3D11(itr->getType());
					mInputElementDescs[idx].InputSlot = itr->getSource();
					mInputElementDescs[idx].AlignedByteOffset = static_cast<WORD>(itr->getOffset());
					mInputElementDescs[idx].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					mInputElementDescs[idx].InstanceDataStepRate = 0;
			}
			mNeedRecreate = false;
		}
	}
}