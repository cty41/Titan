#ifndef __TITAN_SHADER_PARAMS__HH
#define __TITAN_SHADER_PARAMS__HH

#include "TiPrerequisites.h"
#include "TiIteratorWrapper.h"
#include "TiSharedPtr.h"
#include "TiIteratorWrapper.h"

namespace Titan
{
	enum ShaderConstantType
	{
		SCT_FLOAT1 = 1,
		SCT_FLOAT2 = 2,
		SCT_FLOAT3 = 3,
		SCT_FLOAT4 = 4,
		SCT_SAMPLER1D = 5,
		SCT_SAMPLER2D = 6,
		SCT_SAMPLER3D = 7,
		SCT_SAMPLERCUBE = 8,
		SCT_MATRIX_2X2 = 9,
		SCT_MATRIX_2X3 = 10,
		SCT_MATRIX_2X4 = 11,
		SCT_MATRIX_3X2 = 12,
		SCT_MATRIX_3X3 = 13,
		SCT_MATRIX_3X4 = 14,
		SCT_MATRIX_4X2 = 15,
		SCT_MATRIX_4X3 = 16,
		SCT_MATRIX_4X4 = 17,
		SCT_INT1 = 18,
		SCT_INT2 = 19,
		SCT_INT3 = 20,
		SCT_INT4 = 21,
		SCT_UNUSED = 99,
	};

	struct _DllExport ShaderConstantDef
	{
		ShaderConstantType	type;
		size_t physicalIndex;	//memory pos
		size_t	registerIndex;	//gpu register pos

		size_t elementSize; //the size of a single element
		size_t arraySize;	//how many elements

		bool isFloat() const
		{ return isFloat(type); }

		static bool isFloat(ShaderConstantType type)
		{
			switch(type)
			{
			case SCT_INT1:
			case SCT_INT2:
			case SCT_INT3:
			case SCT_INT4:
			case SCT_SAMPLER1D:
			case SCT_SAMPLER2D:
			case SCT_SAMPLER3D:
			case SCT_SAMPLERCUBE:
				return false;
			default:
				return true;
			}
		}

		bool isSampler() const
		{
			return isSampler(type);
		}

		static bool isSampler(ShaderConstantType c)
		{
			switch(c)
			{
			case SCT_SAMPLER1D:
			case SCT_SAMPLER2D:
			case SCT_SAMPLER3D:
			case SCT_SAMPLERCUBE:
				return true;
			default:
				return false;
			};
		}

		static size_t getElementSize(ShaderConstantType ctype, bool padToMultiplesOf4)
		{
			if (padToMultiplesOf4)
			{
				switch(ctype)
				{
				case SCT_FLOAT1:
				case SCT_INT1:
				case SCT_SAMPLER1D:
				case SCT_SAMPLER2D:
				case SCT_SAMPLER3D:
				case SCT_SAMPLERCUBE:
				case SCT_FLOAT2:
				case SCT_INT2:
				case SCT_FLOAT3:
				case SCT_INT3:
				case SCT_FLOAT4:
				case SCT_INT4:
					return 4;
				case SCT_MATRIX_2X2:
				case SCT_MATRIX_2X3:
				case SCT_MATRIX_2X4:
					return 8; // 2 float4s
				case SCT_MATRIX_3X2:
				case SCT_MATRIX_3X3:
				case SCT_MATRIX_3X4:
					return 12; // 3 float4s
				case SCT_MATRIX_4X2:
				case SCT_MATRIX_4X3:
				case SCT_MATRIX_4X4:
					return 16; // 4 float4s
				default:
					return 4;
				};
			}
			else
			{
				switch(ctype)
				{
				case SCT_FLOAT1:
				case SCT_INT1:
				case SCT_SAMPLER1D:
				case SCT_SAMPLER2D:
				case SCT_SAMPLER3D:
				case SCT_SAMPLERCUBE:
					return 1;
				case SCT_FLOAT2:
				case SCT_INT2:
					return 2;
				case SCT_FLOAT3:
				case SCT_INT3:
					return 3;
				case SCT_FLOAT4:
				case SCT_INT4:
					return 4;
				case SCT_MATRIX_2X2:
					return 4;
				case SCT_MATRIX_2X3:
				case SCT_MATRIX_3X2:
					return 6;
				case SCT_MATRIX_2X4:
				case SCT_MATRIX_4X2:
					return 8; 
				case SCT_MATRIX_3X3:
					return 9;
				case SCT_MATRIX_3X4:
				case SCT_MATRIX_4X3:
					return 12; 
				case SCT_MATRIX_4X4:
					return 16; 
				default:
					return 4;
				};

			}
		}

		ShaderConstantDef()
			: type(SCT_UNUSED),registerIndex(0), physicalIndex(0), elementSize(0), arraySize(0){}
	};

	typedef map<String, ShaderConstantDef>::type ShaderConstantDefMap;
	typedef ConstMapIterator<ShaderConstantDefMap> ShaderConstantDefIterator;

	struct _DllExport ShaderNamedConstants : public GeneralAlloc
	{
	public:
		/// Total size of the float buffer required
		size_t floatBufferSize;
		/// Total size of the int buffer required
		size_t intBufferSize;

		ShaderConstantDefMap constantDefMap;

		ShaderNamedConstants(): floatBufferSize(0), intBufferSize(0){}

		void genConstantDefArrayEntries(const String& paramName, const ShaderConstantDef& baseDef);

	};

	typedef SharedPtr<ShaderNamedConstants> ShaderNamedConstantsPtr;

	struct _DllExport ShaderRegisterIndexUse
	{
		/// Physical buffer index
		size_t physicalIndex;
		/// Current physical size allocation
		size_t currentSize;
		ShaderRegisterIndexUse() 
			: physicalIndex(0), currentSize(0){}
		ShaderRegisterIndexUse(size_t bufIdx, size_t curSz) 
			: physicalIndex(bufIdx), currentSize(curSz){}
	};
	typedef map<size_t, ShaderRegisterIndexUse>::type ShaderRegisterIndexUseMap;

	struct _DllExport ShaderRegisterBuffer : public GeneralAlloc
	{
		ShaderRegisterIndexUseMap bufferMap;

		size_t bufferSize;
		ShaderRegisterBuffer(): bufferSize(0){}
	};

	typedef SharedPtr<ShaderRegisterBuffer> ShaderRegisterBufferPtr;


	typedef vector<float>::type FloatConstantVec;

	typedef vector<int>::type IntConstantVec;


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
			///camera position
			ACT_CAMERA_POSITION,
			///custom parameter
			ACT_CUSTOM,
		};


	enum ElementType
	{
		ET_INT,
		ET_FLOAT,
	};
	enum ExtraDataType
	{
		EDT_NONE,
		EDT_INT,
		EDT_FLOAT
	};

	struct AutoConstantDef
	{
		AutoConstantType	constantType;
		String				name;
		ElementType			elementType;
		uint				count;
		ExtraDataType		edType;

		AutoConstantDef(const String& name, AutoConstantType act, ElementType et, uint count,ExtraDataType edt)
			: name(name), constantType(act), elementType(et), count(count), edType(edt)
		{}

	};


	/** Structure recording the use of an automatic parameter. */
		class AutoConstantEntry
		{
		public:
			/// The type of parameter
			AutoConstantType paramType;
			/// The target (physical) constant index
			size_t physicalIndex;
			/** The number of elements per individual entry in this constant
			Used in case people used packed elements smaller than 4 (e.g. GLSL)
			and bind an auto which is 4-element packed to it */
			size_t elementCount;
			/// Additional information to go with the parameter
			union{
				size_t data;
				float fData;
			};


			AutoConstantEntry(AutoConstantType theType, size_t theIndex,size_t theData, size_t theElemCount = 4)
				: paramType(theType), physicalIndex(theIndex), data(theData), elementCount(theElemCount){}

			AutoConstantEntry(AutoConstantType theType, size_t theIndex,float theData, size_t theElemCount = 4)
				: paramType(theType), physicalIndex(theIndex), fData(theData), elementCount(theElemCount){}

		};
		typedef vector<AutoConstantEntry>::type AutoConstantVec;




	public:
		ShaderParams();

		~ShaderParams();
		
		bool	hasAutoConstants() const { return (!mAutoConstants.empty());}

		const AutoConstantVec&	getAutoConstantVec() const { return mAutoConstants; }

		bool	hasFloatConstants() const { return (!mFloatConstants.empty());}

		bool	hasIntConstants() const { return (!mIntConstants.empty()); }

		const ShaderRegisterBufferPtr&	getFloatRegisterBuffer() const { return mFloatRegisterToPhysical; }

		const ShaderRegisterBufferPtr&	getIntRegisterBuffer() const { return mIntRegisterToPhysical; }

		const FloatConstantVec& getFloatConstantVec() const { return mFloatConstants; }

		float* getFloatPtr(size_t pos){ return &mFloatConstants[pos]; }

		const float* getFloatPtr(size_t pos) const { return &mFloatConstants[pos]; }

		const IntConstantVec& getIntConstantVec() const { return mIntConstants; }

		int* getIntPtr(size_t pos){ return &mIntConstants[pos]; }

		const int* getIntPtr(size_t pos) const { return &mIntConstants[pos]; }

		void	updateAutoParams(AutoParamsSetter* setter);

		void	_setRegisterIndexes(const ShaderRegisterBufferPtr& floatIndexes,
			const ShaderRegisterBufferPtr& intIndexes);

		bool	hasNamedParams() const { return !mNamedConstants.isNull(); }

		static const AutoConstantDef* getAutoConstantDef(const String&  name);

		static const AutoConstantDef* getAutoConstantDef(size_t idx);

		static size_t getNumAutoConstantDefs();

		const ShaderConstantDef* _findNamedConstantDefinition(
			const String& name, bool throwExceptionIfMissing = false) const;

		void setNamedConstant(const String& name, float val);

		void setNamedConstant(const String& name, int val);

		void setNamedConstant(const String& name, const Vector4& vec);

		void setNamedConstant(const String& name, const Vector3& vec);

		void setNamedConstant(const String& name, const Matrix4& m);

		void setNamedConstant(const String& name, const Matrix4* m, size_t numEntries);

		void setNamedConstant(const String& name, const float *val, size_t count, 
			size_t multiple = 4);

		void setNamedConstant(const String& name, const Color& color);

		void setNamedConstant(const String& name, const int *val, size_t count, 
			size_t multiple = 4);


		void setNamedAutoConstant(const String& name, AutoConstantType acType,size_t extraInfo = 0);

		void setNamedAutoConstant(const String& name, AutoConstantType acType, float extraInfo);

		void _setRawAutoConstant(size_t physicalIndex, AutoConstantType acType, size_t extraInfo, size_t elementSize = 4);

		void _setRawAutoConstant(size_t physicalIndex, AutoConstantType acType, float extraInfo, size_t elementSize = 4);

		void _setNamedConstants(const ShaderNamedConstantsPtr& constants);

		void _writeRawConstants(size_t physicalIndex, const float* val, size_t count);

		void _writeRawConstants(size_t physicalIndex, const int* val, size_t count);

		void _readRawConstants(size_t physicalIndex, size_t count, float* dest);

		void _readRawConstants(size_t physicalIndex, size_t count, int* dest);

		void _writeRawConstant(size_t physicalIndex, const Vector4& vec, 
			size_t count = 4);

		void _writeRawConstant(size_t physicalIndex, float val);

		void _writeRawConstant(size_t physicalIndex, int val);

		void _writeRawConstant(size_t physicalIndex, const Vector3& vec);

		void _writeRawConstant(size_t physicalIndex, const Matrix4& m, size_t elementCount);
		
		void _writeRawConstant(size_t physicalIndex, const Matrix4* m, size_t numEntries);

		void _writeRawConstant(size_t physicalIndex, const Color& col, size_t count = 4);

		void copyConstants(const ShaderParams& src);

	protected:

		ShaderRegisterIndexUse*	_getFloatConstantRegisterIndexUse(size_t registerIndex, size_t requestSize);

		ShaderRegisterIndexUse*	_getIntConstantRegisterIndexUse(size_t registerIndex, size_t requestSize);

	protected:

		static AutoConstantDef		gAutoConstantDefs[];


		FloatConstantVec			mFloatConstants;
		IntConstantVec				mIntConstants;

		ShaderRegisterBufferPtr		mFloatRegisterToPhysical;
		ShaderRegisterBufferPtr		mIntRegisterToPhysical;

		ShaderNamedConstantsPtr		mNamedConstants;
		AutoConstantVec				mAutoConstants;

		bool mIgnoreMissingParams;

	};


	typedef SharedPtr<ShaderParams>	ShaderParamsPtr;
}


#endif



