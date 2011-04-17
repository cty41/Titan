#ifndef __TITAN_SHADERUNIT__HH
#define __TITAN_SHADERUNIT__HH

#include "TiPrerequisites.h"
#include "TiShader.h"
#include "TiShaderParams.h"

namespace Titan
{
	class _DllExport ShaderUnit : public GeneralAlloc 
	{
	public:
		ShaderUnit(Pass* parent, ShaderType type);

		~ShaderUnit(){}

		ShaderType getType() const { return mType; }

		void	setShader(const String& name);

		const ShaderPtr& getShader() const { return mShader; }
		
		const ShaderParamsPtr&	getShaderParams() const { return mShaderParams; }

		void	_load();


	protected:
		Pass*			mParent;
		ShaderType		mType;
		ShaderPtr		mShader;
		ShaderParamsPtr	mShaderParams;
	};
}

#endif