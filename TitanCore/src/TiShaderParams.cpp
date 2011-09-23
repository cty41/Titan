#include "TitanStableHeader.h"
#include "TiShaderParams.h"
#include "TiStringFuncs.h"
#include "TiAutoParamsSetter.h"
#include "TiRenderable.h"

namespace Titan
{
	ShaderParams::AutoConstantDef ShaderParams::gAutoConstantDefs[] = {
		AutoConstantDef("world", ACT_WORLD_MATRIX, ET_FLOAT, 16, EDT_NONE),
		AutoConstantDef("view", ACT_VIEW_MATRIX, ET_FLOAT, 16, EDT_NONE),
		AutoConstantDef("projection", ACT_PROJECTION_MATRIX, ET_FLOAT, 16, EDT_NONE),
		AutoConstantDef("view_proj", ACT_VIEWPROJ_MATRIX, ET_FLOAT, 16, EDT_NONE),
		AutoConstantDef("world_wiew", ACT_WORLDVIEW_MATRIX, ET_FLOAT, 16, EDT_NONE),
		AutoConstantDef("world_view_proj", ACT_WORLDVIEWPROJ_MATRIX, ET_FLOAT, 16, EDT_NONE),
		AutoConstantDef("camera_position", ACT_CAMERA_POSITION, ET_FLOAT, 3, EDT_NONE),
		AutoConstantDef("custom", ACT_CUSTOM, ET_FLOAT, 4, EDT_INT),
	};

	void ShaderNamedConstants::genConstantDefArrayEntries(const String& paramName, const ShaderConstantDef& baseDef)
	{
		// Copy definition for use with arrays
		ShaderConstantDef arrayDef = baseDef;
		arrayDef.arraySize = 1;
		String arrayName;

		// Add parameters for array accessors
		// [0] will refer to the same location, [1+] will increment
		// only populate others individually up to 16 array slots so as not to get out of hand,
		// unless the system has been explicitly configured to allow all the parameters to be added

		// paramName[0] version will always exist 
		size_t maxArrayIndex = 1;
		if (baseDef.arraySize <= 16)
			maxArrayIndex = baseDef.arraySize;

		for (size_t i = 0; i < maxArrayIndex; i++)
		{
			arrayName = paramName + "[" + StringConverter::toString(i) + "]";
			constantDefMap.insert(ShaderConstantDefMap::value_type(arrayName, arrayDef));
			// increment location
			arrayDef.physicalIndex += arrayDef.elementSize;
		}
		// note no increment of buffer sizes since this is shared with main array def
	}

	ShaderParams::ShaderParams()
		:mIgnoreMissingParams(false)
	{
	}
	//-------------------------------------------------------------------------------//
	ShaderParams::~ShaderParams()
	{

	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, float val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, const Vector4& vec, size_t count)
	{
		_writeRawConstants(physicalIndex, vec.ptr(), std::min(count, (size_t)4));
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, int val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, const Vector3& vec)
	{
		_writeRawConstants(physicalIndex,vec.ptr(), 3);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, const Matrix4& m, size_t elementCount)
	{
		_writeRawConstants(physicalIndex, m[0], elementCount > 16 ? 16: elementCount);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, const Matrix4* m, size_t numEntries)
	{
		_writeRawConstants(physicalIndex, m[0][0], 16 *	numEntries);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstant(size_t physicalIndex, const Color& col, size_t count)
	{
		_writeRawConstants(physicalIndex, col.ptr(), count);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstants(size_t physicalIndex, const float* val, size_t count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(&mFloatConstants[physicalIndex], val, sizeof(float) * count);
	}
	//------------------------------------------------------------------------------//
	inline void ShaderParams::_writeRawConstants(size_t physicalIndex, const int* val, size_t count)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(&mIntConstants[physicalIndex], val, sizeof(int) * count);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::_setRegisterIndexes(const ShaderRegisterBufferPtr& floatIndexes, const ShaderRegisterBufferPtr& intIndexes)
	{
		mFloatRegisterToPhysical = floatIndexes;
		mIntRegisterToPhysical = intIndexes;

		// resize the internal buffers
		// Note that these will only contain something after the first parameter
		// set has set some parameters

		// Size and reset buffer (fill with zero to make comparison later ok)
		if (!floatIndexes.isNull() && floatIndexes->bufferSize > mFloatConstants.size())
		{
			mFloatConstants.insert(mFloatConstants.end(), 
				floatIndexes->bufferSize - mFloatConstants.size(), 0.0f);
		}
		if (!intIndexes.isNull() &&  intIndexes->bufferSize > mIntConstants.size())
		{
			mIntConstants.insert(mIntConstants.end(), 
				intIndexes->bufferSize - mIntConstants.size(), 0);
		}
	}
	//------------------------------------------------------------------------------//
	const ShaderParams::AutoConstantDef* ShaderParams::getAutoConstantDef(const String& name)
	{
		// find a constant definition that matches name by iterating through the 
		// constant definition array
		bool nameFound = false;
		size_t i = 0;
		const size_t numDefs = getNumAutoConstantDefs();
		while (!nameFound && (i < numDefs))
		{
			if (name == gAutoConstantDefs[i].name) 
				nameFound = true;
			else
				++i;
		}

		if (nameFound)
			return &gAutoConstantDefs[i];
		else
			return NULL;
	}
	//------------------------------------------------------------------------------//
	const ShaderParams::AutoConstantDef* ShaderParams::getAutoConstantDef(size_t idx)
	{
		if (idx < getNumAutoConstantDefs())
		{
			// verify index is equal to acType
			// if they are not equal then the dictionary was not setup properly
			assert(idx == static_cast<size_t>(gAutoConstantDefs[idx].constantType));
			return &gAutoConstantDefs[idx];
		}
		else
			return NULL;
	}
	//------------------------------------------------------------------------------//
	size_t ShaderParams::getNumAutoConstantDefs()
	{
		return sizeof(gAutoConstantDefs) / sizeof(AutoConstantDef);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::_setNamedConstants(const ShaderNamedConstantsPtr& constants)
	{
		mNamedConstants = constants;

		// Size and reset buffer (fill with zero to make comparison later ok) ??
		if(constants->floatBufferSize > mFloatConstants.size())
		{
			mFloatConstants.insert(mFloatConstants.end(), 
				constants->floatBufferSize - mFloatConstants.size(), 0.0f);
		}
		if (constants->intBufferSize > mIntConstants.size())
		{
			mIntConstants.insert(mIntConstants.end(), 
				constants->intBufferSize - mIntConstants.size(), 0);
		}
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedAutoConstant(const String& name, AutoConstantType acType,size_t extraInfo)
	{
		const ShaderConstantDef* def = _findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if(def != NULL)
		{
			//add sth later

			ShaderRegisterIndexUse*	indexUse = _getFloatConstantRegisterIndexUse(def->registerIndex,  def->elementSize * def->arraySize);
#if 0
			//for variability
			if (indexUse)
			{

			}
#endif
			_setRawAutoConstant(indexUse->physicalIndex, acType, extraInfo, def->elementSize);
		}
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedAutoConstant(const String& name, AutoConstantType acType, float extraInfo)
	{
		const ShaderConstantDef* def = _findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if(def != NULL)
		{
			//add sth later

			ShaderRegisterIndexUse*	indexUse = _getFloatConstantRegisterIndexUse(def->registerIndex,  def->elementSize * def->arraySize);
#if 0
			//for variability
			if (indexUse)
			{

			}
#endif
			_setRawAutoConstant(indexUse->physicalIndex, acType, extraInfo, def->elementSize);
		}
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::_setRawAutoConstant(size_t physicalIndex, AutoConstantType acType, size_t extraInfo, size_t elementSize)
	{
		// update existing index if it exists
		bool found = false;
		for (AutoConstantVec::iterator i = mAutoConstants.begin(); 
			i != mAutoConstants.end(); ++i)
		{
			if (i->physicalIndex == physicalIndex)
			{
				i->paramType = acType;
				i->elementCount = elementSize;
				i->data = extraInfo;

				found = true;
				break;
			}
		}
		if (!found)
			mAutoConstants.push_back(AutoConstantEntry(acType, physicalIndex, extraInfo, elementSize));
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::_setRawAutoConstant(size_t physicalIndex, AutoConstantType acType, float extraInfo, size_t elementSize)
	{
		// update existing index if it exists
		bool found = false;
		for (AutoConstantVec::iterator i = mAutoConstants.begin(); 
			i != mAutoConstants.end(); ++i)
		{
			if (i->physicalIndex == physicalIndex)
			{
				i->paramType = acType;
				i->elementCount = elementSize;
				i->fData = extraInfo;

				found = true;
				break;
			}
		}
		if (!found)
			mAutoConstants.push_back(AutoConstantEntry(acType, physicalIndex, extraInfo, elementSize));
	}
	//------------------------------------------------------------------------------//
	const ShaderConstantDef* ShaderParams::_findNamedConstantDefinition(const String& name, bool throwExceptionIfMissing) const
	{
		if (mNamedConstants.isNull())
		{
			if (throwExceptionIfMissing)
			{
				TITAN_EXCEPT_INVALIDPARAMS( "Named constants is empty.");
			}
			return NULL;
		}

		ShaderConstantDefMap::const_iterator it = mNamedConstants->constantDefMap.find(name);
		if (it == mNamedConstants->constantDefMap.end())
		{
			if (throwExceptionIfMissing)
			{
				TITAN_EXCEPT_ITEMLOST( "param called" + name + "does not exist.");
			}
			return NULL;
		}
		else
		{
			return &(it->second);
		}
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, float val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, val);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, int val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, val);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, const Vector4& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec, def->elementSize);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, const Vector3& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, const Matrix4& m)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, def->elementSize);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, const Matrix4* m, 
		size_t numEntries)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, numEntries);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, 
		const float *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, const Color& color)
	{
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, color, def->elementSize);
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::setNamedConstant(const String& name, 
		const int *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const ShaderConstantDef* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//------------------------------------------------------------------------------//
	ShaderRegisterIndexUse* ShaderParams::_getFloatConstantRegisterIndexUse(size_t registerIndex, size_t requestSize)
	{
		//quick return
		if (mFloatRegisterToPhysical.isNull())
			return NULL;

		ShaderRegisterIndexUse* indexUse = NULL;
		ShaderRegisterIndexUseMap::iterator rit = mFloatRegisterToPhysical->bufferMap.find(registerIndex);
		if(rit == mFloatRegisterToPhysical->bufferMap.end())
		{
			if (requestSize > 0)
			{
				size_t registerIndex = mFloatConstants.size();
				// Expand at buffer end
				mFloatConstants.insert(mFloatConstants.end(), requestSize, 0.0f);
				// Record extended size for future GPU params re-using this information
				mFloatRegisterToPhysical->bufferSize = mFloatConstants.size();
				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				size_t currPhys = registerIndex;
				size_t count = requestSize / 4;
				ShaderRegisterIndexUseMap::iterator insertedIterator;
				for (size_t logicalNum = 0; logicalNum < count; ++logicalNum)
				{
					ShaderRegisterIndexUseMap::iterator it = 
						mFloatRegisterToPhysical->bufferMap.insert(
						ShaderRegisterIndexUseMap::value_type(
						registerIndex + logicalNum, 
						ShaderRegisterIndexUse(currPhys, requestSize))).first;
					currPhys += 4;

					if (logicalNum == 0)
						insertedIterator = it;
				}
				indexUse = &(insertedIterator->second);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			size_t physicalIndex = rit->second.physicalIndex;
			indexUse = &(rit->second);
			//check size
			if (rit->second.currentSize < requestSize)
			{	
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				size_t insertCount = requestSize - rit->second.currentSize;
				FloatConstantVec::iterator insertPos = mFloatConstants.begin();
				std::advance(insertPos, physicalIndex);
				mFloatConstants.insert(insertPos, insertCount, 0.0f);
				// shift all physical positions after this one
				for (ShaderRegisterIndexUseMap::iterator i = mFloatRegisterToPhysical->bufferMap.begin();
					i != mFloatRegisterToPhysical->bufferMap.end(); ++i)
				{
					if (i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				mFloatRegisterToPhysical->bufferSize += insertCount;
				for (AutoConstantVec::iterator i = mAutoConstants.begin();
					i != mAutoConstants.end(); ++i)
				{
					if (i->physicalIndex > physicalIndex &&
						getAutoConstantDef(i->paramType)->elementType == ET_FLOAT)
					{
						i->physicalIndex += insertCount;
					}
				}
				if (!mNamedConstants.isNull())
				{
					for (ShaderConstantDefMap::iterator i = mNamedConstants->constantDefMap.begin();
						i != mNamedConstants->constantDefMap.end(); ++i)
					{
						if (i->second.isFloat() && i->second.physicalIndex > physicalIndex)
							i->second.physicalIndex += insertCount;
					}
					mNamedConstants->floatBufferSize += insertCount;
				}

				rit->second.currentSize += insertCount;
			}
		}

		return indexUse;
	}
	//------------------------------------------------------------------------------//
	ShaderRegisterIndexUse* ShaderParams::_getIntConstantRegisterIndexUse(size_t registerIndex, size_t requestSize)
	{
		return NULL;
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::copyConstants(const ShaderParams& src)
	{
		mFloatConstants = src.getFloatConstantVec();
		mIntConstants = src.getIntConstantVec();
		mAutoConstants = src.getAutoConstantVec();
	}
	//------------------------------------------------------------------------------//
	void ShaderParams::updateAutoParams(AutoParamsSetter* setter)
	{
		//quick return
		if(!hasAutoConstants())
			return;

		AutoConstantVec::const_iterator it = mAutoConstants.begin(), itend = mAutoConstants.end();
		for(;it != itend;++it)
		{

			switch(it->paramType)
			{
			case ACT_WORLD_MATRIX:
				_writeRawConstant(it->physicalIndex, setter->getWorldMatrix(), it->elementCount);
				break;
			case ACT_VIEW_MATRIX:
				_writeRawConstant(it->physicalIndex, setter->getViewMatrix(), it->elementCount);
				break;
			case ACT_PROJECTION_MATRIX:
				_writeRawConstant(it->physicalIndex, setter->getProjMatrix(), it->elementCount);
				break;
			case ACT_VIEWPROJ_MATRIX:
				_writeRawConstant(it->physicalIndex, setter->getViewProjMatrix(), it->elementCount);
				break;
			case ACT_WORLDVIEW_MATRIX:
				_writeRawConstant(it->physicalIndex, setter->getWorldViewMatrix(),it->elementCount);
				break;
			case ACT_WORLDVIEWPROJ_MATRIX:
				_writeRawConstant(it->physicalIndex, setter->getWorldViewProjMatrix(), it->elementCount);
				break;
			case ACT_CAMERA_POSITION:
				_writeRawConstant(it->physicalIndex, setter->getCameraPosition(), it->elementCount);
				break;
			case ACT_CUSTOM:
				setter->getRenderable()->updateCustomShaderParams(*it, this);
				break;
			default: break;
			}

		}

	}
	//------------------------------------------------------------------------------//
}
