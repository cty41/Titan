#ifndef __TITAN_SHADER__HH
#define __TITAN_SHADER__HH

#include "TiPrerequisites.h"
#include "TiResource.h"
#include "TiSharedPtr.h"
#include "TiShaderParams.h"


namespace Titan
{
	enum ShaderType
	{
		ST_VERTEX_SHADER,
		ST_PIXEL_SHADER,
		ST_GEOMETRY_SHADER,
	};
	//now we only support high level shader ,not support assemble code
	class _DllExport Shader : public Resource
	{
	public:
		Shader(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual);

		virtual ~Shader(){}


		virtual ShaderParamsPtr createShaderParams();

		ShaderParamsPtr	getShaderParams();

		const ShaderNamedConstants& getConstantDefs();

		void	setShaderType(ShaderType st){ mType = st;}

		ShaderType getShaderType() const { return mType; }

		void	setLanguage(const String& language){ mLanguage = language ;}

		const String& getLanguage() const { return mLanguage; }

		void	setFileName(const String& fileName);

		const String& getFileName() const { return mFile; }

		void	setSource(const String& source);

		const String&  getSource() const { return mSource; }

		bool	hasCompiledError() const{ return mHasCompiledError; }

		void	setParamsNames(ShaderParamsPtr params);


	protected:
		void loadImpl();

		virtual void loadFromSrc() = 0;

		virtual void buildConstantDefs() = 0;

		/// Create the internal params logical & named mapping structures
		void createParamMappingBuffer(bool recreateIfExists = true);
		/// Create the internal params logical mapping structures
		void createRegisterParamMappingBuffer(bool recreateIfExists = true);
		/// Create the internal params named mapping structures
		void createNamedParamMappingBuffer(bool recreateIfExists = true);

	protected:

		ShaderParamsPtr	mShaderParams;
		ShaderNamedConstantsPtr mConstantDefs;
		String		mFile;	//load from file
		String		mSource;	//load from String
		String		mLanguage;
		ShaderType	mType;

		bool		mHasCompiledError;
		bool		mConstantDefsBuilt;
		bool		mLoadFromFile;

		ShaderRegisterBufferPtr	mFloatRegisterToPhysical;
		ShaderRegisterBufferPtr	mIntRegisterToPhysical;
	
	protected:
		
		class _DllExport ShaderFileCmd : public ParamsCommand
		{
		public:
			virtual String getter(void* target);

			virtual void setter(void* target, const String& val);
		};

		class _DllExport ShaderSrcCmd : public ParamsCommand
		{
		public:
			virtual String getter(void* target);

			virtual void setter(void* target, const String& val);
		};

		class _DllExport ShaderLanguageCmd : public ParamsCommand
		{
		public:
			virtual String getter(void* target);

			virtual void setter(void* target, const String& val);
		};

		static ShaderFileCmd	msShaderFileCmd;
		static ShaderSrcCmd		msShaderSrcCmd;
		static ShaderLanguageCmd msShaderLanguageCmd;

		//implement from Reflector class
		virtual void setupParamsCmd();
	};

	class _DllExport ShaderPtr : public SharedPtr<Shader>
	{
	public:
		ShaderPtr()
			:SharedPtr<Shader>(){}

		explicit ShaderPtr(Shader* mtrl)
			:SharedPtr<Shader>(mtrl){}

		ShaderPtr(const ShaderPtr& r)
			:SharedPtr<Shader>(r){}

		ShaderPtr(const ResourcePtr& r)
			:SharedPtr<Shader>()
		{
			pRep = static_cast<Shader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}

		ShaderPtr& operator=(const ResourcePtr& r)
		{
			if(pRep == static_cast<Shader*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<Shader*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}
	};
}
#endif