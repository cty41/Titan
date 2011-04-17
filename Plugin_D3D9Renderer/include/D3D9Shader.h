#ifndef	_TITAN_D3D9_SHADER__HH
#define _TITAN_D3D9_SHADER__HH

#include "D3D9Prerequisites.h"
#include "TiShader.h"

namespace Titan
{
	class _D3D9DllExport D3D9Shader : public Shader
	{
	public:
		D3D9Shader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group);

		~D3D9Shader();

		void	setShaderEntry(const String& entry);

		const String& getShaderEntry() const { return mEntryName; }

		void	setTarget(const String&  target){ mTarget = target;}

		const String&  getTarget() const { return mTarget; }

	protected:
		void	loadFromSrc();

		virtual void	unloadImpl();

		void	buildConstantDefs();

		void	parseParam(D3DXHANDLE parent, String prefix, unsigned int index);

		void	populateDef(D3DXCONSTANT_DESC& d3dDesc, ShaderConstantDef& def);

		virtual void _loadShaderFromBuffer() = 0;

	protected:
		String	mEntryName;
		String	mTarget;
		LPD3DXCONSTANTTABLE mpConstTable;
		LPD3DXBUFFER	mShaderBuffer;
		
	};

	class _D3D9DllExport D3D9ShaderPtr : public SharedPtr<D3D9Shader>
	{
	public:
		D3D9ShaderPtr()
			:SharedPtr<D3D9Shader>(){}

		explicit D3D9ShaderPtr(D3D9Shader* mtrl)
			:SharedPtr<D3D9Shader>(mtrl){}

		D3D9ShaderPtr(const D3D9ShaderPtr& r)
			:SharedPtr<D3D9Shader>(r){}

		D3D9ShaderPtr(const ShaderPtr& r)
			:SharedPtr<D3D9Shader>()
		{
			pRep = static_cast<D3D9Shader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}

		D3D9ShaderPtr& operator=(const ShaderPtr& r)
		{
			if(pRep == static_cast<D3D9Shader*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<D3D9Shader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}

		D3D9ShaderPtr(const ResourcePtr& r)
			:SharedPtr<D3D9Shader>()
		{
			pRep = static_cast<D3D9Shader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}

		D3D9ShaderPtr& operator=(const ResourcePtr& r)
		{
			if(pRep == static_cast<D3D9Shader*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<D3D9Shader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}
	};

	class _D3D9DllExport D3D9VertexShader : public D3D9Shader
	{
	public:
		D3D9VertexShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group);

		~D3D9VertexShader();

		IDirect3DVertexShader9*	getVertexShader() const { return mpVertexShader; }

	protected:
		virtual void _loadShaderFromBuffer();
	protected:
		IDirect3DVertexShader9*		mpVertexShader;
	};

	class _D3D9DllExport D3D9PixelShader : public D3D9Shader
	{
	public:
		D3D9PixelShader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group);

		~D3D9PixelShader();

		IDirect3DPixelShader9*	getPixelShader() const { return mpPixelShader;}
	
	protected:
		virtual void _loadShaderFromBuffer();

	protected:
		IDirect3DPixelShader9*	mpPixelShader;

	};
}

#endif