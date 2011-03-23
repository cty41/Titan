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
		mNamedConstantParams.push_back(NamedConstantParam(name,handle,type, sizeInByte));

	}
	//-------------------------------------------------------------//
	ShaderParams::AutoConstantParamVecIterator ShaderParams::getAutoConstantParamVecIterator()
	{
		return AutoConstantParamVecIterator(mAutoConstantParams.begin(), mAutoConstantParams.end());
	}
	//-------------------------------------------------------------//
	ShaderParams::NamedConstantParamVecIterator	 ShaderParams::getNamedConstantParamVecIterator()
	{
		return NamedConstantParamVecIterator(mNamedConstantParams.begin(), mNamedConstantParams.end());
	}
	//-------------------------------------------------------------//
	uint	ShaderParams::getNameParamIndex(const String& name)
	{
		ShaderParams::NamedConstantParamVec::iterator it = mNamedConstantParams.begin(), itend = mNamedConstantParams.end();
		uint idx = 0;
		while(it != itend)
		{
			if((*it).name == name)
				return idx;
			++idx;
			++it;
		}
		return -1;
	}
}
