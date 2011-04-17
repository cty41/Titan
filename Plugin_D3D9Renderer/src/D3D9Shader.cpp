#include "D3D9Shader.h"
#include "TiException.h"
#include "TiStringFuncs.h"
#include "D3D9Renderer.h"


namespace Titan
{
	D3D9Shader::D3D9Shader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:Shader(mgr, name, id, group), mpConstTable(NULL), mEntryName("vs_basic"), mTarget("vs_1_1")
	{
	}
	//----------------------------------------------------------------------------//
	D3D9Shader::~D3D9Shader()
	{
		if (isLoaded())
		{
			unload();
		}
	}
	//----------------------------------------------------------------------------//
	void D3D9Shader::loadFromSrc()
	{
		LPD3DXBUFFER pErr = NULL;
		
		HRESULT hr = D3DXCompileShader(mSource.c_str(), static_cast<UINT>(mSource.length()),
			 NULL,
			 NULL,
			 mEntryName.c_str(),
			 mTarget.c_str(),
			 D3DXSHADER_PACKMATRIX_COLUMNMAJOR,
			 &mShaderBuffer,
			 &pErr,
			 &mpConstTable);

		if(FAILED(hr))
		{
			String message = "Cannot compile D3D9 high-level shader " + mName + " Errors:\n" +
				static_cast<const char*>(pErr->GetBufferPointer());
			pErr->Release();
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, message,
				"D3D9Shader::loadFromSrc");
		}

		_loadShaderFromBuffer();
	}
	//------------------------------------------------------------------------------//
	void D3D9Shader::unloadImpl()
	{
		SAFE_RELEASE(mShaderBuffer);
		SAFE_RELEASE(mpConstTable);
		mConstantDefsBuilt = false;
	}
	//----------------------------------------------------------------------------//
	void D3D9Shader::buildConstantDefs()
	{
		assert(mpConstTable && "we do not load the shader");

		D3DXCONSTANTTABLE_DESC desc;
		HRESULT hr = mpConstTable->GetDesc(&desc);

		if(FAILED(hr))
		{
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"we can not get constants descriptions from shader",
				"D3D9Shader::buildConstantDefs");
		}

		createParamMappingBuffer(true);
		//parse every constant
		for (uint i = 0; i < desc.Constants; ++i)
		{
			parseParam(NULL, "", i);
		}
	}
	//----------------------------------------------------------------------------//
	void D3D9Shader::parseParam(D3DXHANDLE parent, String prefix, unsigned int index)
	{
		D3DXHANDLE hConst = mpConstTable->GetConstant(parent, index);

		D3DXCONSTANT_DESC desc;
		uint numParams = 1;
		HRESULT hr = mpConstTable->GetConstantDesc(hConst, &desc, &numParams);
		if(FAILED(hr))
		{
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"we can not get constants descriptions from shader",
				"D3D9Shader::parseParam");
		}

		String paramName = desc.Name;
		// trim the odd '$' which appears at the start of the names in HLSL
		if (paramName.at(0) == '$')
			paramName.erase(paramName.begin());

		// Also trim the '[0]' suffix if it exists, we will add our own indexing later
		if (StringUtil::endsWith(paramName, "[0]", false))
		{
			paramName.erase(paramName.size() - 3);
		}

		if (desc.Class == D3DXPC_STRUCT)
		{
			prefix = prefix + paramName + ".";
			for(uint i = 0;i < desc.StructMembers; ++i)
			{
				parseParam(hConst, prefix, i);
			}
		}
		else
		{
			if (desc.Type == D3DXPT_FLOAT || desc.Type == D3DXPT_INT || desc.Type == D3DXPT_BOOL)
			{
				size_t paramIndex = desc.RegisterIndex;
				String name = prefix + paramName;

				ShaderConstantDef def;
				def.registerIndex = paramIndex;
				populateDef(desc, def);
				if (def.isFloat())
				{
					def.physicalIndex = mFloatRegisterToPhysical->bufferSize;
					mFloatRegisterToPhysical->bufferMap.insert(ShaderRegisterIndexUseMap::value_type(
						paramIndex, 
						ShaderRegisterIndexUse(def.physicalIndex,def.arraySize * def.elementSize)));
					mFloatRegisterToPhysical->bufferSize += def.arraySize * def.elementSize;
					mConstantDefs->floatBufferSize = mFloatRegisterToPhysical->bufferSize;
				}
				else
				{
					def.physicalIndex = mIntRegisterToPhysical->bufferSize;
					mIntRegisterToPhysical->bufferMap.insert(ShaderRegisterIndexUseMap::value_type(
						paramIndex, 
						ShaderRegisterIndexUse(def.physicalIndex,def.arraySize * def.elementSize)));
					mIntRegisterToPhysical->bufferSize += def.arraySize * def.elementSize;
					mConstantDefs->floatBufferSize = mIntRegisterToPhysical->bufferSize;
				}

				mConstantDefs->constantDefMap.insert(ShaderConstantDefMap::value_type(name, def));

				mConstantDefs->genConstantDefArrayEntries(name, def);




			}
		}

	}
	//----------------------------------------------------------------------------//
	void D3D9Shader::populateDef(D3DXCONSTANT_DESC& d3dDesc, ShaderConstantDef& def)
	{
		def.arraySize = d3dDesc.Elements;
		switch(d3dDesc.Type)
		{
		case D3DXPT_INT:
			switch(d3dDesc.Columns)
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
			break;
		case D3DXPT_FLOAT:
			switch(d3dDesc.Class)
			{
			case D3DXPC_MATRIX_COLUMNS:
			case D3DXPC_MATRIX_ROWS:
				{
					int firstDim, secondDim;
					firstDim = d3dDesc.RegisterCount / d3dDesc.Elements;
					if (d3dDesc.Class == D3DXPC_MATRIX_ROWS)
					{
						secondDim = d3dDesc.Columns;
					}
					else
					{
						secondDim = d3dDesc.Rows;
					}
					switch(firstDim)
					{
					case 2:
						switch(secondDim)
						{
						case 2:
							def.type = SCT_MATRIX_2X2;
							def.elementSize = 8; // HLSL always packs
							break;
						case 3:
							def.type = SCT_MATRIX_2X3;
							def.elementSize = 8; // HLSL always packs
							break;
						case 4:
							def.type = SCT_MATRIX_2X4;
							def.elementSize = 8; 
							break;
						} // columns
						break;
					case 3:
						switch(secondDim)
						{
						case 2:
							def.type = SCT_MATRIX_3X2;
							def.elementSize = 12; // HLSL always packs
							break;
						case 3:
							def.type = SCT_MATRIX_3X3;
							def.elementSize = 12; // HLSL always packs
							break;
						case 4:
							def.type = SCT_MATRIX_3X4;
							def.elementSize = 12; 
							break;
						} // columns
						break;
					case 4:
						switch(secondDim)
						{
						case 2:
							def.type = SCT_MATRIX_4X2;
							def.elementSize = 16; // HLSL always packs
							break;
						case 3:
							def.type = SCT_MATRIX_4X3;
							def.elementSize = 16; // HLSL always packs
							break;
						case 4:
							def.type = SCT_MATRIX_4X4;
							def.elementSize = 16; 
							break;
						} // secondDim
						break;

					} // firstDim
				}
				break;
			case D3DXPC_SCALAR:
			case D3DXPC_VECTOR:
				switch(d3dDesc.Columns)
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
			}
		default:
			// not mapping samplers, don't need to take the space 
			break;
		};

		// D3D9 pads to 4 elements
		def.elementSize = ShaderConstantDef::getElementSize(def.type, true);
	}



	//----------------------------------------------------------------------------//
	D3D9VertexShader::D3D9VertexShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:D3D9Shader(mgr, name, id, group), mpVertexShader(NULL)
	{
	}
	//----------------------------------------------------------------------------//
	D3D9VertexShader::~D3D9VertexShader()
	{
		SAFE_RELEASE(mpVertexShader);
	}
	//------------------------------------------------------------------------------//
	void D3D9VertexShader::_loadShaderFromBuffer()
	{
		HRESULT hr ;
		LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
		hr = pD3D9Device->CreateVertexShader(static_cast<DWORD*>(mShaderBuffer->GetBufferPointer()), &mpVertexShader);
		if(FAILED(hr))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"create vertex shader failed because of: " + errMsg,
				"D3D9VertexShader::_loadShaderFromBuffer()");
		}
	}


	//----------------------------------------------------------------------------//
	D3D9PixelShader::D3D9PixelShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:D3D9Shader(mgr, name, id, group), mpPixelShader(NULL)
	{

	}
	//----------------------------------------------------------------------------//
	D3D9PixelShader::~D3D9PixelShader()
	{
		SAFE_RELEASE(mpPixelShader);
	}
	//------------------------------------------------------------------------------//
	void D3D9PixelShader::_loadShaderFromBuffer()
	{
		HRESULT hr ;
		LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
		hr = pD3D9Device->CreatePixelShader(static_cast<DWORD*>(mShaderBuffer->GetBufferPointer()), &mpPixelShader);
		if(FAILED(hr))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"create pixel shader failed because of: " + errMsg,
				"D3D9PixelShader::_loadShaderFromBuffer()");
		}
	}

}