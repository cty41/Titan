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


	protected:
		void			loadImpl();

		void			unloadImpl();
	protected:
		ID3DXEffect*		mEffect;
	};
}

#endif