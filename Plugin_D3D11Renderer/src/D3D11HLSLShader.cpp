#include "D3D11HLSLShader.h"
#include "D3D11Renderer.h"


namespace Titan
{
	D3D11HLSLShader::D3D11EntryCmd D3D11HLSLShader::msD3D11EntryCmd;
	D3D11HLSLShader::D3D11TargetCmd D3D11HLSLShader::msD3D11TargetCmd;
	
	D3D11HLSLShader::D3D11HLSLShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual)
		:Shader(mgr, name, id, group, isManual),
		mCompiledShader(nullptr), mReflection(nullptr), mConstantBuffer(nullptr), mReflectionConstantBuffer(nullptr)
	{
		if(createClassParamsDict("shader"))
			D3D11HLSLShader::setupParamsCmd();
	}

	D3D11HLSLShader::~D3D11HLSLShader()
	{
		if(isLoaded())
			unload();
	}

	void D3D11HLSLShader::unloadImpl()
	{
		SAFE_RELEASE(mConstantBuffer);
		SAFE_DELETE(mReflectionConstantBuffer);
		SAFE_RELEASE(mReflection);
		SAFE_RELEASE(mCompiledShader);

	}

	void D3D11HLSLShader::loadFromSrc()
	{
		HRESULT hr;
		ID3DBlob* pErrMsg = nullptr;
		UINT flag = 0;

		//todo, to add inlcude and macro support
#ifdef _DEBUG
		flag |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		hr = D3DX11CompileFromMemory(mSource.c_str(), mSource.size(), 
			nullptr,nullptr,nullptr, 
			mEntryName.c_str(), mTarget.c_str(),
			flag,0,
			nullptr,	//id3dx11threadpump
			&mCompiledShader,
			&pErrMsg, 
			nullptr);

		if(FAILED(hr))
		{
			String errStr = (const char*)pErrMsg->GetBufferPointer();
			pErrMsg->Release();
			TITAN_EXCEPT_API("d3d11 compile shader failed : " + errStr);
			return ;
		}

		hr = D3DReflect((void*)mCompiledShader, mCompiledShader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&mReflection);
		if(FAILED(hr))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("D3Dx11 create Shader Reflection failed : " + errMsg);
			return ;
		}

		//create shader descs, todo
		mReflection->GetDesc(&mShaderDesc);

		// I do not support more than one constant buffer, now is no need
		if(mShaderDesc.ConstantBuffers == 1)
		{
			mReflectionConstantBuffer = mReflection->GetConstantBufferByIndex(0);

			mReflectionConstantBuffer->GetDesc(&mConstantBufferDesc);

			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.ByteWidth = mConstantBufferDesc.Size;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;

			ID3D11Device* pd3dDevice = D3D11Renderer::getSingletonPtr()->__getD3D11Device();
			hr = pd3dDevice->CreateBuffer(&bufferDesc, 0 ,&mConstantBuffer);
			if(FAILED(hr))
			{
				String errMsg = DXGetErrorDescription(hr);
				TITAN_EXCEPT_API("D3DX11 Shader Constant buffer creation failed: " + errMsg);
				return ;
			}

		}
		else
		{
			//error
			TITAN_EXCEPT_API("D3DX11:now we only support one constant buffer!!");
			return ;
		}

		//create shader object
		_loadShaderFromBuffer();
	}

	void D3D11HLSLShader::buildConstantDefs()
	{
		if(mConstantBuffer)
		{
			createParamMappingBuffer(true);

			if(mReflectionConstantBuffer)
			{
				
				for(uint i = 0 ;i < mConstantBufferDesc.Variables; ++i)
				{
					ID3D11ShaderReflectionVariable* pShaderReflectionVar = nullptr;
					pShaderReflectionVar = mReflectionConstantBuffer->GetVariableByIndex(i);

					ID3D11ShaderReflectionType* varRefType = nullptr;
					varRefType = pShaderReflectionVar->GetType();
					parseParam("", pShaderReflectionVar, varRefType);
				}
			}
			
		}
	}

	void D3D11HLSLShader::parseParam(const String& prefix, ID3D11ShaderReflectionVariable* shaderReflectionVar, ID3D11ShaderReflectionType* shaderReflectionType)
	{
		D3D11_SHADER_VARIABLE_DESC varDesc;
		shaderReflectionVar->GetDesc(&varDesc);

		D3D11_SHADER_TYPE_DESC typeDesc;
		shaderReflectionType->GetDesc(&typeDesc);


		if(typeDesc.Type == D3D10_SVT_BOOL || typeDesc.Type == D3D10_SVT_INT || typeDesc.Type == D3D10_SVT_FLOAT)
		{
			String varName = prefix + varDesc.Name;

			ShaderConstantDef def;
			
			populateDef(typeDesc, def);

			if(def.isFloat())
			{
				def.physicalIndex = mFloatRegisterToPhysical->bufferSize;
				mFloatRegisterToPhysical->bufferMap.insert(ShaderRegisterIndexUseMap::value_type(
					def.registerIndex,									//pos in constant buffer
					ShaderRegisterIndexUse(def.physicalIndex,			//pos in memory
							def.arraySize * def.elementSize)			//full size of this variable
					));
				mFloatRegisterToPhysical->bufferSize += def.arraySize * def.elementSize;
				mConstantDefs->floatBufferSize = mFloatRegisterToPhysical->bufferSize;
			}
			else
			{
				def.physicalIndex = mIntRegisterToPhysical->bufferSize;
				mIntRegisterToPhysical->bufferMap.insert(ShaderRegisterIndexUseMap::value_type(
					def.registerIndex,									//pos in constant buffer
					ShaderRegisterIndexUse(def.physicalIndex,			//pos in memory
							def.arraySize * def.elementSize)));			//full size of this variable
				mIntRegisterToPhysical->bufferSize += def.arraySize * def.elementSize;
				mConstantDefs->intBufferSize = mIntRegisterToPhysical->bufferSize;
			}

			mConstantDefs->constantDefMap.insert(ShaderConstantDefMap::value_type(varName, def));

			mConstantDefs->genConstantDefArrayEntries(varName, def);
		}

	}

	void D3D11HLSLShader::populateDef(const D3D11_SHADER_TYPE_DESC& desc, ShaderConstantDef& def)
	{
		def.arraySize = desc.Elements;
		def.registerIndex = desc.Offset;
		if(desc.Type == D3D10_SVT_INT)
		{
			switch(desc.Columns)
			{
			case 1:
				def.type = SCT_INT1;
				break;
			case 2:
				def.type = SCT_INT2;
				break;
			case 3:
				def.type = SCT_INT3;
				break;
			case 4:
				def.type = SCT_INT4;
				break;
			} // columns
		}
		else if(desc.Type == D3D10_SVT_FLOAT)
		{
			switch(desc.Rows)
			{
			case 1:
				switch(desc.Columns)
				{
				case 1:
					def.type = SCT_FLOAT1;
					break;
				case 2:
					def.type = SCT_FLOAT2;
					break;
				case 3:
					def.type = SCT_FLOAT3;
					break;
				case 4:
					def.type = SCT_FLOAT4;
					break;
				} // columns
				break;
			case 2:
				switch(desc.Columns)
				{
				case 2:
					def.type = SCT_MATRIX_2X2;
					break;
				case 3:
					def.type = SCT_MATRIX_2X3;
					break;
				case 4:
					def.type = SCT_MATRIX_2X4;
					break;
				} // columns
				break;
			case 3:
				switch(desc.Columns)
				{
				case 2:
					def.type = SCT_MATRIX_3X2;
					break;
				case 3:
					def.type = SCT_MATRIX_3X3;
					break;
				case 4:
					def.type = SCT_MATRIX_3X4;
					break;
				} // columns
				break;
			case 4:
				switch(desc.Columns)
				{
				case 2:
					def.type = SCT_MATRIX_4X2;
					break;
				case 3:
					def.type = SCT_MATRIX_4X3;
					break;
				case 4:
					def.type = SCT_MATRIX_4X4;
					break;
				} // columns
				break;

			} // rows
		}

		def.elementSize = ShaderConstantDef::getElementSize(def.type, true);
	}


	ID3D11Buffer* D3D11HLSLShader::getConstBuffer(ShaderParamsPtr params)
	{
		D3D11_MAPPED_SUBRESOURCE pConstData;
		if(mConstantBuffer)
		{
			HRESULT hr;
			ID3D11DeviceContext* pDeviceContext = D3D11Renderer::getSingletonPtr()->__getD3D11DeviceContext();
			hr = pDeviceContext->Map(mConstantBuffer,0, D3D11_MAP_WRITE_DISCARD, 0 , &pConstData);
			if(FAILED(hr))
			{
				TITAN_EXCEP_API_D11(hr, "D3DX11 Map Shader Constant Buffer failed:");
				return nullptr;
			}
			ShaderRegisterBufferPtr floatRegs = params->getFloatRegisterBuffer();
			for (auto i = floatRegs->bufferMap.cbegin();i != floatRegs->bufferMap.cend();++i)
			{
				size_t offset = i->first;
				void* pFloat = (void*)params->getFloatPtr(i->second.physicalIndex);
				memcpy((void*)(((char*)pConstData.pData) + offset), pFloat, i->second.currentSize);
			}

			ShaderRegisterBufferPtr intRegs = params->getIntRegisterBuffer();
			for (auto i = intRegs->bufferMap.cbegin();i != intRegs->bufferMap.cend();++i)
			{
				size_t offset = i->first;
				void* pInt = (void*)params->getIntPtr(i->second.physicalIndex);
				memcpy((void*)(((char*)pConstData.pData) + offset), pInt, i->second.currentSize);
			}

			pDeviceContext->Unmap(mConstantBuffer, 0);
		}

		//should we just message a warning?
		return nullptr;
	}

	//------------------------------------------------------------------------------//
	void D3D11HLSLShader::setupParamsCmd()
	{
		Shader::setupParamsCmd();
		mClassParamsDict->addParamsCommand("target", &msD3D11TargetCmd);
		mClassParamsDict->addParamsCommand("entry", & msD3D11EntryCmd);
	}


	//
	//D11 VERTEX SHADER
	//
	D3D11VertexShader::D3D11VertexShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual)
		:D3D11HLSLShader(mgr, name, id, group, isManual), mpVertexShader(nullptr)
	{
		mType = ST_VERTEX_SHADER;
	}

	D3D11VertexShader::~D3D11VertexShader()
	{
		SAFE_RELEASE(mpVertexShader);
	}

	void D3D11VertexShader::_loadShaderFromBuffer()
	{
		HRESULT hr;
		ID3D11Device *pd3dDevice = D3D11Renderer::getSingleton().__getD3D11Device();
		hr = pd3dDevice->CreateVertexShader(mCompiledShader->GetBufferPointer(), mCompiledShader->GetBufferSize(),
			NULL, //todo, to support dynamic linkage
			&mpVertexShader);
		if(FAILED(hr))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("D3D10 Create Vertex Shader Failed: "+ errMsg);
		}
	}

	//
	//D11 VERTEX SHADER
	//
	D3D11PixelShader::D3D11PixelShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual)
		:D3D11HLSLShader(mgr, name, id, group, isManual), mpPixelShader(nullptr)
	{
		mType = ST_PIXEL_SHADER;
	}

	D3D11PixelShader::~D3D11PixelShader()
	{
		SAFE_RELEASE(mpPixelShader);
	}

	void D3D11PixelShader::_loadShaderFromBuffer()
	{
		HRESULT hr;
		ID3D11Device *pd3dDevice = D3D11Renderer::getSingleton().__getD3D11Device();
		hr = pd3dDevice->CreatePixelShader(mCompiledShader->GetBufferPointer(), mCompiledShader->GetBufferSize(),
			NULL, //todo, to support dynamic linkage
			&mpPixelShader);
		if(FAILED(hr))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API("D3D10 Create Pixel Shader Failed: "+ errMsg);
		}
	}
}