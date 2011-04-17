#ifndef __TITAN_PASS_HH
#define __TITAN_PASS_HH

#include "TiPrerequisites.h"
#include "TiTextureUnit.h"
#include "TitanCommon.h"
#include "TitanBlendMode.h"
#include "TiIteratorWrapper.h"
#include "TiShaderUnit.h"

namespace Titan
{

	class _DllExport Pass : public GeneralAlloc
	{
	public:
		typedef vector<TextureUnit*>::type TextureUnitVec;
		typedef VectorIterator<TextureUnitVec>	TextureUnitVecIterator;

		typedef ConstVectorIterator<TextureUnitVec>	ConstTextureUnitVecIterator;
	public:
		Pass(Material*	parent, size_t index);

		~Pass();

		Material*	getParent() const { return mParent; }

		TextureUnit*	createTextureUnit();

		void			removeAllTextureUnits();

		TextureUnitVecIterator	getTextureUnitVecIterator() { return TextureUnitVecIterator(mTextureUnitVec.begin(), mTextureUnitVec.end());}

		ConstTextureUnitVecIterator	getTextureUnitVecIterator() const{ return ConstTextureUnitVecIterator(mTextureUnitVec.begin(), mTextureUnitVec.end());}

		size_t		getPassIndex() const { return mIndex; }

		bool		isTransparent() const { return !(mDstBlendFactor == SBF_ZERO &&
			mSrcBlendFactor != SBF_DEST_COLOR &&
			mSrcBlendFactor != SBF_ONE_MINUS_DEST_COLOR &&
			mSrcBlendFactor != SBF_DEST_ALPHA &&
			mSrcBlendFactor != SBF_ONE_MINUS_DEST_ALPHA);}

		void		setCullMode(CullingMode m){ mCullMode = m;}

		CullingMode getCullMode() const { return mCullMode; }

		void		setPolygonMode(PolygonMode pm){ mPolygonMode = pm; }

		PolygonMode	getPolygonMode() const { return mPolygonMode; }

		void		setShadeMode(ShadeOptions sm){ mShadeMode = sm;} 

		ShadeOptions getShadeMode() const { return mShadeMode; }

		void		setLightEnable(bool state){ mIsLightEnable = state; }

		bool		IsLightEnable() const { return mIsLightEnable; }

		void		setDepthFunc(CompareFunction cf){ mDepthFunc = cf;}
		
		CompareFunction	getDepthFunc() const { return mDepthFunc; }

		void		setDepthCheck(bool state){ mDepthCheck = state; }

		bool		isDepthCheck() const { return mDepthCheck; }

		void		setDepthWrite(bool state){ mDepthWrite = state; }

		bool		isDepthWritable() const { return mDepthWrite; }

		bool		isTransparentSort() const { return mSortTransparent;}

		void		setTransparentSort(bool state) { mSortTransparent = state; }

		SceneBlendFactor	getSrcBlendFactor() const { return mSrcBlendFactor; }

		void		setSrcBlendFactor(SceneBlendFactor sbf){ mSrcBlendFactor = sbf; }

		SceneBlendFactor	getDstBlendFactor() const { return mDstBlendFactor; }

		void		setDstBlendFactor(SceneBlendFactor sbf){ mDstBlendFactor = sbf; }

		SceneBlendFactor	getSrcBlendFactorAlpha() const { return mSrcBlendFactorAlpha; }

		void		setSrcBlendFactorAlpha(SceneBlendFactor sbf){ mSrcBlendFactorAlpha = sbf; }

		SceneBlendFactor	getDstBlendFactorAlpha() const { return mDstBlendFactorAlpha; }

		void		setDstBlendFactorAlpha(SceneBlendFactor sbf){ mDstBlendFactorAlpha = sbf; }

		SceneBlendOperation getSceneBlendOperation() const { return mBlendOperation; }

		void		setSceneBlendOperation(SceneBlendOperation sbo){ mBlendOperation = sbo; }

		bool		isProgrammable() const {  return (mVertexShaderUnit || mPixelShaderUnit);}

		bool		hasVertexShader() const { return (mVertexShaderUnit? true : false);}

		bool		hasPixelShader() const { return (mPixelShaderUnit? true : false);}

		const ShaderUnit*	getVertexShaderUnit() const { return mVertexShaderUnit;}

		const ShaderUnit*	getPixelShaderUnit() const { return mPixelShaderUnit; }
		
		void		updateAutoParams(AutoParamsSetter* setter) const;

		void		setVertexShader(const String& name);

		const String&	getVertexShaderName() const;

		void		setPixelShader(const String& name);

		const String&	getPixelShaderName() const;

		const ShaderParamsPtr&	getVertexShaderParams() const { return mVertexShaderUnit->getShaderParams();}

		const ShaderParamsPtr&	getPixelShaderParams() const { return mPixelShaderUnit->getShaderParams();}

		void		_load();

		void		_unload();

	protected:
		Material*		mParent;
		TextureUnitVec	mTextureUnitVec;
		uint32			mHashCode;
		size_t			mIndex;

		CullingMode		mCullMode;
		PolygonMode		mPolygonMode;
		ShadeOptions	mShadeMode;

		// Blending factors
		SceneBlendFactor mSrcBlendFactor;
		SceneBlendFactor mDstBlendFactor;
		SceneBlendFactor mSrcBlendFactorAlpha;
		SceneBlendFactor mDstBlendFactorAlpha;

		// Blending operations
		SceneBlendOperation mBlendOperation;
		SceneBlendOperation mAlphaBlendOperation;

		// Depth buffer settings
		bool mDepthCheck;
		bool mDepthWrite;
		CompareFunction mDepthFunc;

		bool	mSortTransparent;
		bool	mIsLightEnable;

		ShaderUnit*		mVertexShaderUnit;
		ShaderUnit*		mPixelShaderUnit;
	};
}

#endif