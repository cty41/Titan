#include "TitanStableHeader.h"
#include "TitanShaderParams.h"

namespace Titan
{
	ShaderParams::AutoConstantParamDef ShaderParams::gAutoConstantParamDefs[] = {
		AutoConstantParamDef("World", ACT_WORLD_MATRIX, ET_FLOAT, 16),
		AutoConstantParamDef("View", ACT_VIEW_MATRIX, ET_FLOAT, 16),
		AutoConstantParamDef("Projection", ACT_PROJECTION_MATRIX, ET_FLOAT, 16),
		AutoConstantParamDef("ViewProj", ACT_VIEWPROJ_MATRIX, ET_FLOAT, 16),
		AutoConstantParamDef("WorldView", ACT_WORLDVIEW_MATRIX, ET_FLOAT, 16),
		AutoConstantParamDef("WorldViewProj", ACT_WORLDVIEWPROJ_MATRIX, ET_FLOAT, 16),
	};

	ShaderParams::ShaderParams()
	{
	}
	//-------------------------------------------------------------//
	ShaderParams::~ShaderParams()
	{

	}
	//-------------------------------------------------------------//
	void ShaderParams::addParam(const String& name, const String& semantic, uint handle, ShaderConstantType type, uint sizeInByte)
	{
		uint autoConstantNum = sizeof(gAutoConstantParamDefs) / sizeof(AutoConstantParamDef);
		for(uint i = 0; i < autoConstantNum; ++i)
		{
			if(gAutoConstantParamDefs[i].name == semantic)
			{
				//add autoConstant
				mAutoConstantParams.push_back(AutoConstantParam(gAutoConstantParamDefs[i].constantType, handle));
				return ;
			}
		}
		//to add custom params here
	}
	//-------------------------------------------------------------//
	ShaderParams::AutoConstantParamVecIterator ShaderParams::getAutoConstantParamVecIterator()
	{
		return AutoConstantParamVecIterator(mAutoConstantParams.begin(), mAutoConstantParams.end());
	}
}
