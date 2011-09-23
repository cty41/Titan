#include "TitanStableHeader.h"
#include "TiPass.h"
#include "TiTextureUnit.h"
#include "TiStringFuncs.h"
#include "TiMaterial.h"

namespace Titan
{
	Pass::CullModeCmd	Pass::msCullModeCmd;
	Pass::PolygonModeCmd  Pass::msPolygonModeCmd;
	Pass::ShadeModeCmd  Pass::msShadeModeCmd;
	Pass::SrcBlendCmd	Pass::msSrcBlendCmd;
	Pass::DstBlendCmd	Pass::msDstBlendCmd;
	Pass::SrcBlendAlphaCmd  Pass::msSrcBlendAlphaCmd;
	Pass::DstBlendAlphaCmd  Pass::msDstBlendAlphaCmd;
	Pass::BlendOperationCmd	Pass::msBlendOperationCmd;
	Pass::AlphaBlendOperationCmd	Pass::msAlphaBlendOperationCmd;
	Pass::DepthCheckCmd	Pass::msDepthCheckCmd;
	Pass::DepthWriteCmd	Pass::msDepthWriteCmd;
	Pass::DepthFuncCmd	Pass::msDepthFuncCmd;
	Pass::SortTransparentCmd	Pass::msSortTransparentCmd;
	Pass::LightEnableCmd	Pass::msLightEnableCmd;

	//we need to change here mSrcBlendFactor and mDstlendFactor
	Pass::Pass(Material* parent, size_t index)
		:mParent(parent), mIndex(index),mHashCode(0),
		mCullMode(CULL_CLOCKWISE), mPolygonMode(PM_SOLID), 
		mShadeMode(SO_GOURAUD), mSrcBlendFactor(SBF_ONE), 
		mDstBlendFactor(SBF_ZERO),
		mSrcBlendFactorAlpha(SBF_ONE), mDstBlendFactorAlpha(SBF_ZERO),
		mBlendOperation(SBO_ADD), mAlphaBlendOperation(SBO_ADD), mDepthCheck(true),
		mDepthWrite(true),mDepthFunc(CMPF_LESS_EQUAL), mSortTransparent(true),
		mIsLightEnable(false), mVertexShaderUnit(NULL), mPixelShaderUnit(NULL)
	{
		if(createClassParamsDict("pass"))
			Pass::setupParamsCmd();
	}
	//-------------------------------------------------------------------------------//
	Pass::~Pass()
	{
		removeAllTextureUnits();

		if(mVertexShaderUnit)
			TITAN_DELETE mVertexShaderUnit;

		if(mPixelShaderUnit)
			TITAN_DELETE mPixelShaderUnit;
	}
	//-------------------------------------------------------------------------------//
	TextureUnit* Pass::createTextureUnit()
	{
		TextureUnit* tu = TITAN_NEW TextureUnit(this);
		mTextureUnitVec.push_back(tu);

		return tu;
	}
	//-------------------------------------------------------------------------------//
	void Pass::removeAllTextureUnits()
	{
		TextureUnitVec::iterator it = mTextureUnitVec.begin(), itend = mTextureUnitVec.end();
		for(;it != itend;++it)
		{
			TITAN_DELETE (*it);
		}
		mTextureUnitVec.clear();
	}
	//-------------------------------------------------------------------------------//
	void Pass::updateAutoParams(AutoParamsSetter* setter) const
	{
		if(hasVertexShader())
		{
			mVertexShaderUnit->getShaderParams()->updateAutoParams(setter);
		}

		if(hasPixelShader())
		{
			mPixelShaderUnit->getShaderParams()->updateAutoParams(setter);
		}
	}
	//------------------------------------------------------------------------------//
	const String&  Pass::getVertexShaderName() const
	{
		if (!mVertexShaderUnit)
		{
			return StringUtil::BLANK;
		}
		else
			return mVertexShaderUnit->getShader()->getName();
	}
	//------------------------------------------------------------------------------//
	const String&  Pass::getPixelShaderName() const
	{
		if (!mPixelShaderUnit)
		{
			return StringUtil::BLANK;
		}
		else
			return mPixelShaderUnit->getShader()->getName();
	}
	//------------------------------------------------------------------------------//
	void Pass::setVertexShader(const String& name)
	{
		if (getVertexShaderName() != name)
		{
			if(name.empty())
			{
				if(mVertexShaderUnit)
					TITAN_DELETE mVertexShaderUnit;
				mVertexShaderUnit = NULL;
			}
			else
			{
				if (!mVertexShaderUnit)
				{
					mVertexShaderUnit = TITAN_NEW ShaderUnit(this, ST_VERTEX_SHADER);
				}
				mVertexShaderUnit->setShader(name);
			}
		}
		else
		{
			if(!mVertexShaderUnit)
			{
				mVertexShaderUnit = TITAN_NEW ShaderUnit(this, ST_VERTEX_SHADER);
			}
			mVertexShaderUnit->setShader(name);
		}
	}
	//------------------------------------------------------------------------------//
	void Pass::setPixelShader(const String& name)
	{
		if (getPixelShaderName() != name)
		{
			if(name.empty())
			{
				if(mPixelShaderUnit)
					TITAN_DELETE mPixelShaderUnit;
				mPixelShaderUnit = NULL;
			}
			else
			{
				if (!mPixelShaderUnit)
				{
					mPixelShaderUnit = TITAN_NEW ShaderUnit(this, ST_PIXEL_SHADER);
				}
				mPixelShaderUnit->setShader(name);
			}
		}
		else
		{
			if(!mPixelShaderUnit)
			{
				mPixelShaderUnit = TITAN_NEW ShaderUnit(this, ST_PIXEL_SHADER);
			}
			mPixelShaderUnit->setShader(name);
		}
	}
	//------------------------------------------------------------------------------//
	void Pass::_load()
	{
		TextureUnitVec::iterator it = mTextureUnitVec.begin(), itend = mTextureUnitVec.end();
		for(;it != itend;++it)
			(*it)->_load();

		if(mVertexShaderUnit)
			mVertexShaderUnit->_load();

		if(mPixelShaderUnit)
			mPixelShaderUnit->_load();
	}
	//-------------------------------------------------------------------------------//
	void Pass::_unload()
	{
		TextureUnitVec::iterator it = mTextureUnitVec.begin(), itend = mTextureUnitVec.end();
		for(;it != itend;++it)
			(*it)->_unload();

		if(mVertexShaderUnit)
			mVertexShaderUnit->_unload();

		if(mPixelShaderUnit)
			mPixelShaderUnit->_unload();
	}
	//------------------------------------------------------------------------------//
	bool Pass::isLoaded() const 
	{
		return mParent->isLoaded();
	}
	//------------------------------------------------------------------------------//
	const String&  Pass::getGroup() const
	{
		return mParent->getGroup();
	}
	//------------------------------------------------------------------------------//
	TextureUnit* Pass::getTextureUnit(uint idx)
	{
		assert(idx < mTextureUnitVec.size() && " invalid texture unit index");
		return mTextureUnitVec[idx];
	}
	//------------------------------------------------------------------------------//
	void Pass::setupParamsCmd()
	{
		mClassParamsDict->addParamsCommand("cull_mode", &msCullModeCmd);
		mClassParamsDict->addParamsCommand("polygon_mode", &msPolygonModeCmd);
		mClassParamsDict->addParamsCommand("shade_mode", &msShadeModeCmd);
		mClassParamsDict->addParamsCommand("src_blend", &msSrcBlendCmd);
		mClassParamsDict->addParamsCommand("dst_blend", &msDstBlendCmd);
		mClassParamsDict->addParamsCommand("src_blend_alpha", &msSrcBlendAlphaCmd);
		mClassParamsDict->addParamsCommand("dst_blend_alpha", &msDstBlendAlphaCmd);
		mClassParamsDict->addParamsCommand("blend_operation", &msBlendOperationCmd);
		mClassParamsDict->addParamsCommand("alpha_blend_operation", &msAlphaBlendOperationCmd);
		mClassParamsDict->addParamsCommand("depth_write", &msDepthWriteCmd);
		mClassParamsDict->addParamsCommand("depth_check", &msDepthCheckCmd);
		mClassParamsDict->addParamsCommand("depth_func", &msDepthFuncCmd);
		mClassParamsDict->addParamsCommand("sort_transparent", &msSortTransparentCmd);
		mClassParamsDict->addParamsCommand("lighting", &msLightEnableCmd);
	}

	//
	//all the setter
	//
	void Pass::CullModeCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "none")
		{
			pass->setCullMode(CULL_NONE);
		}
		else if (val == "clockwise")
		{
			pass->setCullMode(CULL_CLOCKWISE);
		}
		else if (val == "counter_clockwise")
		{
			pass->setCullMode(CULL_COUNTERCLOCKWISE);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"cull_mode value includes 'off' , 'clockwise' or 'counter_clockwise', not " + val);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::CullModeCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		switch(pass->getCullMode())
		{
		case  CULL_CLOCKWISE:
			return "clockwise";
		case CULL_COUNTERCLOCKWISE:
			return "counter_clockwise";
		case CULL_NONE:
		default:
			return "none";
		}
		return "";
	}
	//------------------------------------------------------------------------------//
	void Pass::PolygonModeCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "solid")
		{
			pass->setPolygonMode(PM_SOLID);
		}
		else if (val == "wireframe")
		{
			pass->setPolygonMode(PM_WIREFRAME);
		}
		else if (val == "points")
		{
			pass->setPolygonMode(PM_POINTS);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"polygon mode value includes 'solid' , 'wireframe' or 'points', not " + val);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::PolygonModeCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		switch(pass->getPolygonMode())
		{
		case PM_SOLID:
			return "solid";
		case PM_WIREFRAME:
			return "wireframe";
		case PM_POINTS:
			return "points";
		}
		return "";
	}
	//------------------------------------------------------------------------------//
	void Pass::ShadeModeCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "flat")
		{
			pass->setShadeMode(SO_FLAT);
		}
		else if(val == "gouraud")
		{
			pass->setShadeMode(SO_GOURAUD);
		}
		else if(val == "phong")
		{
			pass->setShadeMode(SO_PHONG);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"shader mode value includes 'flat' , 'gouraud' or 'phong', not " + val);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::ShadeModeCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		switch(pass->getShadeMode())
		{
		case  SO_FLAT:
			return "flat";
		case SO_GOURAUD:
			return "gouraud";
		case SO_PHONG:
			return "phong";
		}
		return "";
	}
	//------------------------------------------------------------------------------//
	void Pass::SrcBlendCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		
		int sbf = mapToBlendFactor(val);
		if(sbf != 0x7fffffff)
		{
			pass->setSrcBlendFactor(SceneBlendFactor(sbf));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS("src blend factor error: " + val);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::SrcBlendCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return blendFactorToString(pass->getSrcBlendFactor());
	}
	//------------------------------------------------------------------------------//
	void Pass::DstBlendCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		int sbf = mapToBlendFactor(val);
		if(sbf != 0x7fffffff)
		{
			pass->setDstBlendFactor(SceneBlendFactor(sbf));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"dst blend factor error: " + val);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::DstBlendCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return blendFactorToString(pass->getDstBlendFactor());
	}
	//------------------------------------------------------------------------------//
	void Pass::SrcBlendAlphaCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		int sbf = mapToBlendFactor(val);
		if(sbf != 0x7fffffff)
		{
			pass->setSrcBlendFactorAlpha(SceneBlendFactor(sbf));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"src blend alpha factor error: " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::SrcBlendAlphaCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return blendFactorToString(pass->getSrcBlendFactorAlpha());
	}
	//------------------------------------------------------------------------------//
	void Pass::DstBlendAlphaCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		int sbf = mapToBlendFactor(val);
		if(sbf != 0x7fffffff)
		{
			pass->setDstBlendFactorAlpha(SceneBlendFactor(sbf));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"dst blend alpha factor error: " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::DstBlendAlphaCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return blendFactorToString(pass->getDstBlendFactorAlpha());
	}
	//------------------------------------------------------------------------------//
	void Pass::BlendOperationCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		int sbo = mapToBlendOp(val);
		if(sbo != 0x7fffffff)
		{
			pass->setSceneBlendOperation(SceneBlendOperation(sbo));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"scene blend operation error: " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::BlendOperationCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return blendOpToString(pass->getSceneBlendOperation());
	}
	//------------------------------------------------------------------------------//
	void Pass::AlphaBlendOperationCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		int sbo = mapToBlendOp(val);
		if(sbo != 0x7fffffff)
		{
			pass->setAlphaSceneBlendOperation(SceneBlendOperation(sbo));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"scene blend operation error: " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::AlphaBlendOperationCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return blendOpToString(pass->getAlphaSceneBlendOperation());
	}
	//------------------------------------------------------------------------------//
	void Pass::DepthCheckCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "on")
		{
			pass->setDepthCheck(true);
		}
		else if (val == "off")
		{
			pass->setDepthCheck(false);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"depth_check value includes 'on' or 'off', not " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::DepthCheckCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(pass->isDepthCheck())
			return "on";
		else
			return "off";
	}
	//------------------------------------------------------------------------------//
	void Pass::DepthWriteCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "on")
		{
			pass->setDepthWrite(true);
		}
		else if (val == "off")
		{
			pass->setDepthWrite(false);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"depth_write value includes 'on' or 'off', not " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::DepthWriteCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(pass->isDepthWritable())
			return "on";
		else
			return "off";
	}
	//------------------------------------------------------------------------------//
	void Pass::DepthFuncCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		int cf = mapToCompareFunc(val);
		if(cf != 0x7fffffff)
		{
			pass->setDepthFunc(CompareFunction(cf));
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"depth func error: " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::DepthFuncCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		return compareFuncToString(pass->getDepthFunc());
	}
	//------------------------------------------------------------------------------//
	void Pass::SortTransparentCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "on")
			pass->setTransparentSort(true);
		else if(val == "off")
			pass->setTransparentSort(false);
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"sort_transparent value error: " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::SortTransparentCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(pass->isTransparentSort())
			return "on";
		else
			return "off";
	}
	//------------------------------------------------------------------------------//
	void Pass::LightEnableCmd::setter(void* target, const String& val)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(val == "on")
		{
			pass->setLightEnable(true);
		}
		else if(val == "off")
		{
			pass->setLightEnable(false);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"lighting value includes 'on' or 'off', not " + val
				);
		}
	}
	//------------------------------------------------------------------------------//
	String Pass::LightEnableCmd::getter(void* target)
	{
		Pass* pass = static_cast<Pass*>(target);
		if(pass->IsLightEnable())
			return "on";
		else
			return "off";
	}

	//------------------------------------------------------------------------------//
	SceneBlendFactor Pass::mapToBlendFactor(const String& val)
	{

		if(val == "one")
			return SBF_ONE;
		else if(val == "zero")
			return SBF_ZERO;
		else if(val == "dst_color")
			return SBF_DEST_COLOR;
		else if(val == "src_color")
			return SBF_SOURCE_COLOR;
		else if(val == "one_minus_dst_color")
			return SBF_ONE_MINUS_DEST_COLOR;
		else if(val == "one_minus_src_color")
			return SBF_ONE_MINUS_SOURCE_COLOR;
		else if(val == "dst_alpha")
			return SBF_DEST_ALPHA;
		else if(val == "src_alpha")
			return SBF_SOURCE_ALPHA;
		else if(val == "one_minus_dst_alpha")
			return SBF_ONE_MINUS_DEST_ALPHA;
		else if(val == "one_minus_src_alpha")
			return SBF_ONE_MINUS_SOURCE_ALPHA;
		else
			return SBF_INVALID;
	}
	//------------------------------------------------------------------------------//
	String Pass::blendFactorToString(SceneBlendFactor sbf)
	{
		switch(sbf)
		{
		case SBF_ONE:
			return "one";
		case SBF_ZERO:
			return "zero";
		case SBF_DEST_COLOR:
			return "dst_color";
		case SBF_SOURCE_COLOR:
			return "src_color";
		case SBF_ONE_MINUS_DEST_COLOR:
			return "one_minus_dst_color";
		case SBF_ONE_MINUS_SOURCE_COLOR:
			return "one_minus_src_color";
		case SBF_DEST_ALPHA:
			return "dst_alpha";
		case SBF_SOURCE_ALPHA:
			return "src_alpha";
		case SBF_ONE_MINUS_DEST_ALPHA:
			return "one_minus_dst_alpha";
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return "one_minus_src_alpha";
		}
		return "";
	}
	//------------------------------------------------------------------------------//
	SceneBlendOperation Pass::mapToBlendOp(const String&  val)
	{
		if(val == "add")
			return SBO_ADD;
		else if(val == "subtract")
			return SBO_SUBTRACT;
		else if(val == "reverse_subtract")
			return SBO_REVERSE_SUBTRACT;
		else if(val == "min")
			return SBO_MIN;
		else if(val == "max")
			return SBO_MAX;
		else
			return SBO_INVALID;
	}
	//------------------------------------------------------------------------------//
	String Pass::blendOpToString(SceneBlendOperation sbo)
	{
		switch(sbo)
		{
		case SBO_ADD:
			return "add";
		case SBO_SUBTRACT:
			return "subtract";
		case SBO_REVERSE_SUBTRACT:
			return "reverse_subtract";
		case SBO_MIN:
			return "min";
		case SBO_MAX:
			return "max";
		}
		return "";
	}
	//------------------------------------------------------------------------------//
	CompareFunction Pass::mapToCompareFunc(const String& val)
	{
		if(val == "always_fail")
			return CMPF_ALWAYS_FAIL;
		else if(val == "always_pass")
			return CMPF_ALWAYS_PASS;
		else if(val == "less")
			return CMPF_LESS;
		else if(val == "less_equal")
			return CMPF_LESS_EQUAL;
		else if(val == "equal")
			return CMPF_EQUAL;
		else if(val == "not_equal")
			return CMPF_NOT_EQUAL;
		else if(val == "greater_equal")
			return CMPF_GREATER_EQUAL;
		else if(val == "greater")
			return CMPF_GREATER;
		else
			return CMPF_INVALID;
	}
	//------------------------------------------------------------------------------//
	String Pass::compareFuncToString(CompareFunction cf)
	{
		switch(cf)
		{
			case CMPF_ALWAYS_FAIL:
				return "always_fail";
			case CMPF_ALWAYS_PASS:
				return "always_pass";
			case CMPF_LESS:
				return "less";
			case CMPF_LESS_EQUAL:
				return "less_equal";
			case CMPF_EQUAL:
				return "equal";
			case CMPF_NOT_EQUAL:
				return "not_equal";
			case CMPF_GREATER_EQUAL:
				return "greater_equal";
			case CMPF_GREATER:
				return "greater";
		}
		return "";
	}
}