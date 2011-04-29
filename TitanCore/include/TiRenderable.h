#ifndef __TITAN_RENDERABLE_HH
#define __TITAN_RENDERABLE_HH

#include "TiPrerequisites.h"
#include "TiVector4.h"
#include "TiTexture.h"
#include "TiMaterial.h"

namespace Titan
{
	class _DllExport Renderable : public GeneralAlloc
	{
	public:
		typedef map<size_t, Vector4>::type CustomShaderParams;
	public:
		Renderable(){};

		virtual ~Renderable(){};

		virtual void			getRenderData(RenderData& rd) = 0;

		virtual void			getTransformMat(Matrix4* transMat) = 0;

		virtual RenderData*		getRenderData() = 0;

		virtual const MaterialPtr&	getMaterial() const = 0;

		virtual float			getSquaredDistance(Camera* cam) = 0;

		virtual bool			IsUseIdentityView() const = 0;

		virtual bool			IsUseIdentityProj() const = 0;

		void setCustomShaderParam(size_t idx, const Vector4& value)
		{
			mCustomShaderParams[idx] = value;
		}

		void updateCustomShaderParams(const ShaderParams::AutoConstantEntry& entry, ShaderParams* params)
		{
			CustomShaderParams::const_iterator cit = mCustomShaderParams.find(entry.data);
			if(cit != mCustomShaderParams.end())
			{
				params->_writeRawConstant(entry.physicalIndex, cit->second);
			}
		}

	protected:
		CustomShaderParams	mCustomShaderParams;
	};
}
#endif