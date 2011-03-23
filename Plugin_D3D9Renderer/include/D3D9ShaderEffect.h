#ifndef _D3D9_SHADEREFFECT__HH
#define _D3D9_SHADEREFFECT__HH

#include "D3D9Prerequisites.h"
#include "TitanShaderEffect.h"

namespace Titan
{
	class _D3D9DllExport D3D9ShaderEffect : public ShaderEffect
	{
	public:
		D3D9ShaderEffect(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group);

		~D3D9ShaderEffect();

		void			begin();

		void			updateAutoParams(ShaderParamsUpdater* updater);

		void			end();

		void			setNamedParamByIndex(uint index, const float* pValue);


	protected:
		void			loadImpl();

		void			unloadImpl();

		void			_parseEffectContent();
	protected:
		ID3DXEffect*		mEffect;
		D3DXEFFECT_DESC		mEffectDesc;
		D3DXHANDLE			mTechnique;
		D3DXTECHNIQUE_DESC	mTechniqueDesc;
	};
}

#endif