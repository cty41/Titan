#ifndef __TITAN_SHADER_PARAMS__HH
#define __TITAN_SHADER_PARAMS__HH

#include "TitanPrerequisites.h"
#include "TitanIteratorWrapper.h"

namespace Titan
{
	enum ShaderConstantType
	{
		SCT_INT,
		SCT_FLOAT,
		SCT_TEXTURE1D,
		SCT_TEXTURE2D,
		SCT_TEXTURE3D,
		SCT_TEXTURECUBE,
		SCT_SAMPLER1D,
		SCT_SAMPLER2D,
		SCT_SAMPLER3D,
		SCT_SAMPLERCUBE,
		SCT_UNUSED,
	};


	class _DllExport ShaderParams : public GeneralAlloc
	{
	public:
		enum AutoConstantType
		{
			/// The current world matrix
			ACT_WORLD_MATRIX,
			/// The current view matrix
			ACT_VIEW_MATRIX,
			/// The current projection matrix
			ACT_PROJECTION_MATRIX,
			/// The current view & projection matrices concatenated
			ACT_VIEWPROJ_MATRIX,
			/// The current world & view matrices concatenated
			ACT_WORLDVIEW_MATRIX,
			/// The current world, view & projection matrices concatenated
			ACT_WORLDVIEWPROJ_MATRIX,
		};


	enum ElementType
	{
		ET_INT,
		ET_FLOAT,
	};

	struct AutoConstantParamDef
	{
		AutoConstantType	constantType;
		String				name;
		ElementType			elementType;
		uint				count;

		AutoConstantParamDef(const String& name, AutoConstantType act, ElementType et, uint count)
			: name(name), constantType(act), elementType(et), count(count)
		{}

	};

	struct AutoConstantParam
	{
		AutoConstantType	constantType;
		uint				handle;

		AutoConstantParam(AutoConstantType type, uint handle)
			: constantType(type), handle(handle)
		{
		}
	};

	typedef vector<AutoConstantParam>::type		AutoConstantParamVec;
	typedef VectorIterator<AutoConstantParamVec>	AutoConstantParamVecIterator;

	struct NamedConstantParam
	{
		uint				handle;
		String				name;
		ShaderConstantType	constantType;
		ElementType			elementType;
		uint				count;
		NamedConstantParam(const String& name, uint handle, ShaderConstantType ct, uint count)
			: name(name), handle(handle), constantType(ct), count(count)
		{
		};
	};
	
	typedef vector<NamedConstantParam>::type  NamedConstantParamVec;
	typedef VectorIterator<NamedConstantParamVec>	NamedConstantParamVecIterator;
	public:
		ShaderParams();

		~ShaderParams();

		void	addParam(const String& name, const String& semantic, uint handle, ShaderConstantType type, uint sizeInByte);

		uint	getNameParamIndex(const String& paramSemantic);

		const NamedConstantParam&	getNameParamByIndex(uint index){ return mNamedConstantParams.at(index); }

		AutoConstantParamVecIterator	getAutoConstantParamVecIterator();

		NamedConstantParamVecIterator	getNamedConstantParamVecIterator();

	protected:

	protected:

		static AutoConstantParamDef	gAutoConstantParamDefs[];

		AutoConstantParamVec		mAutoConstantParams;
		NamedConstantParamVec		mNamedConstantParams;



	};
}


#endif



