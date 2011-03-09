#include "TitanStableHeader.h"
#include "TitanShaderEffect.h"

namespace Titan
{
	ShaderEffect::ShaderEffect(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group)
		:Resource(mgr, name, id, group)
	{
	}
	//-------------------------------------------------------------//
	ShaderEffect::~ShaderEffect()
	{

	}
}
