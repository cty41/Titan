#ifndef __D3D11__HLSL_SHADER__HH__
#define __D3D11__HLSL_SHADER__HH__

#include "D3D11Prerequisites.h"
#include "TiShader.h"

namespace Titan
{
	class _D3D11DllExport D3D11HLSLShader : public Shader
	{
	public:
		D3D11HLSLShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual);

		~D3D11HLSLShader();

		void	setShaderEntry(const String& entry){ mEntryName = entry;}

		const String& getShaderEntry() const { return mEntryName; }

		void	setTarget(const String&  target){ mTarget = target;}

		const String&  getTarget() const { return mTarget; }

		ID3DBlob*	getCompiledCode() const { return mCompiledShader; }

		ID3D11Buffer*	getConstBuffer(ShaderParamsPtr params);

	protected:
		void	loadFromSrc();

		virtual void	unloadImpl();

		void	buildConstantDefs();

		void parseParam(const String& prefix, ID3D11ShaderReflectionVariable* shaderReflectionVar, ID3D11ShaderReflectionType* shaderReflectionType);

		void populateDef(const D3D11_SHADER_TYPE_DESC& desc, ShaderConstantDef& def);

		virtual void _loadShaderFromBuffer() = 0;

	protected:
		String	mEntryName;
		String	mTarget;

		ID3DBlob*		mCompiledShader;
		ID3D11Buffer*	mConstantBuffer;
		ID3D11ShaderReflection*					mReflection;
		ID3D11ShaderReflectionConstantBuffer*	mReflectionConstantBuffer;

		D3D11_SHADER_DESC			mShaderDesc;
		D3D11_SHADER_BUFFER_DESC	mConstantBufferDesc;



	protected:
		class _D3D11DllExport D3D11TargetCmd : public ParamsCommand
		{
		public:
			virtual String getter(void* target)
			{
				D3D11HLSLShader* shader = static_cast<D3D11HLSLShader*>(target);
				return shader->getTarget();
			}

			virtual void setter(void* target, const String& val)
			{
				D3D11HLSLShader* shader = static_cast<D3D11HLSLShader*>(target);
				shader->setTarget(val);
			}
		};

		class _D3D11DllExport D3D11EntryCmd : public ParamsCommand
		{
		public:
			virtual String getter(void* target)
			{
				D3D11HLSLShader* shader = static_cast<D3D11HLSLShader*>(target);
				return shader->getShaderEntry();
			}

			virtual void setter(void* target, const String& val)
			{
				D3D11HLSLShader* shader = static_cast<D3D11HLSLShader*>(target);
				shader->setShaderEntry(val);
			}
		};

		static D3D11TargetCmd	msD3D11TargetCmd;
		static D3D11EntryCmd	msD3D11EntryCmd;

		virtual void setupParamsCmd();		
	};

	class _D3D11DllExport D3D11HLSLShaderPtr : public SharedPtr<D3D11HLSLShader>
	{
	public:
		D3D11HLSLShaderPtr()
			:SharedPtr<D3D11HLSLShader>(){}

		explicit D3D11HLSLShaderPtr(D3D11HLSLShader* mtrl)
			:SharedPtr<D3D11HLSLShader>(mtrl){}

		D3D11HLSLShaderPtr(const D3D11HLSLShaderPtr& r)
			:SharedPtr<D3D11HLSLShader>(r){}

		D3D11HLSLShaderPtr(const ShaderPtr& r)
			:SharedPtr<D3D11HLSLShader>()
		{
			pRep = static_cast<D3D11HLSLShader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}

		D3D11HLSLShaderPtr& operator=(const ShaderPtr& r)
		{
			if(pRep == static_cast<D3D11HLSLShader*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<D3D11HLSLShader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}

		D3D11HLSLShaderPtr(const ResourcePtr& r)
			:SharedPtr<D3D11HLSLShader>()
		{
			pRep = static_cast<D3D11HLSLShader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}

		D3D11HLSLShaderPtr& operator=(const ResourcePtr& r)
		{
			if(pRep == static_cast<D3D11HLSLShader*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<D3D11HLSLShader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}
	};

	class _D3D11DllExport D3D11VertexShader : public D3D11HLSLShader
	{
	public:
		D3D11VertexShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual);

		~D3D11VertexShader();

		ID3D11VertexShader*	getVertexShader() const { return mpVertexShader; }

	protected:
		virtual void _loadShaderFromBuffer();
	protected:
		ID3D11VertexShader*		mpVertexShader;
	};

	class _D3D11DllExport D3D11PixelShader : public D3D11HLSLShader
	{
	public:
		D3D11PixelShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual);

		~D3D11PixelShader();

		ID3D11PixelShader*	getPixelShader() const { return mpPixelShader;}

	protected:
		virtual void _loadShaderFromBuffer();

	protected:
		ID3D11PixelShader*	mpPixelShader;

	};
}

#endif