#ifndef _TITAN_SHADER_EFFECT__HH
#define _TITAN_SHADER_EFFECT__HH

#include "TitanPrerequisites.h"
#include "TitanResource.h"
#include "TitanShaderParams.h"

namespace Titan
{

	class _DllExport ShaderEffect : public Resource
	{
	public:
		ShaderEffect(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group);

		virtual ~ShaderEffect();

		virtual void begin() = 0;

		virtual void updateParams(ShaderParamsUpdater* updater);

		virtual void updateAutoParams(ShaderParamsUpdater* updater) = 0;

		virtual void end() = 0;

		virtual void setNamedParamByIndex(uint index, const float* pValue) = 0;

		ShaderParams	getShaderParams()  { return mParams; }

	protected:
		ShaderParams	mParams;

	};



	class ShaderEffectPtr : public SharedPtr<ShaderEffect>
	{
	public:
		ShaderEffectPtr()
			: SharedPtr<ShaderEffect>()
		{}

		explicit ShaderEffectPtr(ShaderEffect* tex, SharedPtrFreeMethod freeMethod = SPFM_DELETE)
			: SharedPtr<ShaderEffect>(tex, freeMethod){}

		ShaderEffectPtr(const ShaderEffectPtr& r) : SharedPtr<ShaderEffect>(r) {} 
		ShaderEffectPtr(const ResourcePtr& r) : SharedPtr<ShaderEffect>()
		{
			pRep = static_cast<ShaderEffect*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}

		}

		/// Operator used to convert a ResourcePtr to a TexturePtr
		ShaderEffectPtr& operator=(const ResourcePtr& r)
		{
			if (pRep == static_cast<ShaderEffect*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<ShaderEffect*>(r.getPointer());
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