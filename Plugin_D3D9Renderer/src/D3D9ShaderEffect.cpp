#include "D3D9ShaderEffect.h"
#include "D3D9Renderer.h"
#include "Exception.h"
#include "D3D9Mappings.h"
#include "TitanShaderParams.h"
#include "TitanShaderParamsUpdater.h"

namespace Titan
{
	D3D9ShaderEffect::D3D9ShaderEffect(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group)
		:ShaderEffect(mgr, name, id, group), mEffect(0)
	{
	}
	//-------------------------------------------------------------//
	D3D9ShaderEffect::~D3D9ShaderEffect()
	{
		SAFE_RELEASE(mEffect);
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::loadImpl()
	{
		HRESULT hr;
		LPDIRECT3DDEVICE9 pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();
		LPD3DXBUFFER	pErrorBuf;
		if(FAILED(hr = D3DXCreateEffectEx(
			pD3D9Device, mPreparedData->getPtr(), mPreparedData->size(),
			NULL, NULL, NULL, D3DXFX_DONOTSAVESTATE, 
			NULL, &mEffect, &pErrorBuf)))
		{
			String errMsg = DXGetErrorDescription(hr);
			//(char*)pErrorBuf->GetBufferPointer(); probably get error here
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
			errMsg + ": " + (char*)pErrorBuf->GetBufferPointer(),
				"D3D9ShaderEffect::loadImpl");
		}

		_parseEffectContent();
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::unloadImpl()
	{
		SAFE_RELEASE(mEffect);
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::_parseEffectContent()
	{
		HRESULT hr;
		mEffect->GetDesc( &mEffectDesc);

		// find the best possible technique
		hr = mEffect->FindNextValidTechnique(NULL, &mTechnique);
		if( FAILED( hr ) )
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"parse effect failed : " + errMsg,
				"D3D9ShaderEffect::_parseEffectContent");
		}

		// get some info about the technique
		hr = mEffect->GetTechniqueDesc(mTechnique, &mTechniqueDesc);
		if( FAILED( hr ) )
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"parse effect failed : " + errMsg,
				"D3D9ShaderEffect::_parseEffectContent");
		}

		// activate it
		hr = mEffect->SetTechnique(mTechnique);
		if( FAILED( hr ) )
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"parse effect failed : " + errMsg,
				"D3D9ShaderEffect::_parseEffectContent");
		}

		
		D3DXPARAMETER_DESC ParamDesc;
		D3DXHANDLE hParam;
		//parse paramters
		for( uint iParam = 0; iParam < mEffectDesc.Parameters; ++iParam )
		{
			hParam = mEffect->GetParameter(NULL, iParam);
			mEffect->GetParameterDesc(hParam, &ParamDesc);
			if(!ParamDesc.Semantic)
				continue;
			mParams.addParam(ParamDesc.Name, ParamDesc.Semantic, (uint)hParam, 
				D3D9Mappings::convertConstantType(ParamDesc.Type),
				ParamDesc.Bytes);

		}

	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::begin()
	{
		HRESULT hr;
		if(FAILED(hr = mEffect->Begin(0, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESHADERSTATE)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				errMsg,
				"D3D9ShaderEffect::begin");
		}
		mEffect->BeginPass(0);
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::updateAutoParams(ShaderParamsUpdater* updater)
	{
		HRESULT hr;
		//to do: optimize later
		ShaderParams::AutoConstantParamVecIterator paramsIterator = mParams.getAutoConstantParamVecIterator();
		while(paramsIterator.hasMoreElements())
		{
			ShaderParams::AutoConstantParam param = paramsIterator.peekNext();
			D3DXHANDLE d3dHandle = D3DXHANDLE(param.handle);
			switch (param.constantType)
			{
			case ShaderParams::ACT_WORLD_MATRIX:
				{
					mEffect->SetMatrix(d3dHandle, &(D3D9Mappings::makeD3DXMatrix(updater->getWorldMatrix(), true)));
					break;
				}
			case ShaderParams::ACT_VIEW_MATRIX:
				{
					mEffect->SetMatrix(d3dHandle, &(D3D9Mappings::makeD3DXMatrix(updater->getViewMatrix(), true)));
					break;
				}
			case ShaderParams::ACT_PROJECTION_MATRIX:
				{
					mEffect->SetMatrix(d3dHandle, &(D3D9Mappings::makeD3DXMatrix(updater->getProjMatrix(), true)));
					break;
				}
			case ShaderParams::ACT_VIEWPROJ_MATRIX:
				{
					mEffect->SetMatrix(d3dHandle, &(D3D9Mappings::makeD3DXMatrix(updater->getProjMatrix() * updater->getViewMatrix(), true)));
					break;
				}

			case ShaderParams::ACT_WORLDVIEW_MATRIX:
				{
					mEffect->SetMatrix(d3dHandle, &(D3D9Mappings::makeD3DXMatrix(updater->getViewMatrix() * updater->getWorldMatrix(), true)));
					break;
				}
			case ShaderParams::ACT_WORLDVIEWPROJ_MATRIX:
				{
					mEffect->SetMatrix(d3dHandle, &(D3D9Mappings::makeD3DXMatrix(updater->getProjMatrix() * updater->getViewMatrix() * updater->getWorldMatrix(), true)));
					break;
				}
				//to add other auto constants later when we need
			default:
				break;
			}
			paramsIterator.next();
		}


		if(FAILED(hr = mEffect->CommitChanges()))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"change commit failed " + errMsg,
				"D3D9ShaderEffect::updateParams");
		}

	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::end()
	{
		HRESULT hr;
		mEffect->EndPass();
		if(FAILED(hr = mEffect->End()))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"ID3DXEffect failed: " + errMsg,
				"D3D9ShaderEffect::end");
		}
	}
	//-------------------------------------------------------------//
	void D3D9ShaderEffect::setNamedParamByIndex(uint index, const float* pValue)
	{
		HRESULT hr;
		ShaderParams::NamedConstantParam namedParam = mParams.getNameParamByIndex(index);
		if(FAILED(hr = mEffect->SetValue(D3DXHANDLE(namedParam.handle), pValue, namedParam.count)))
		{
			String errMsg = DXGetErrorDescription(hr);
			TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR,
				"Set Named Parameter failed because of: " + errMsg,
				"D3D9ShaderEffect::setNamedParamByIndex");
		}
	}
}