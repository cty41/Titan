#include "TitanStableHeader.h"
#include "TitanShaderParamsUpdater.h"


namespace Titan
{
	ShaderParamsUpdater::ShaderParamsUpdater()
		:mCurrentRenderable(0), mCamera(0)
	{
		mWorldMatrix = Matrix4::IDENTITY;
	}
	//-------------------------------------------------------------------------------//
	ShaderParamsUpdater::~ShaderParamsUpdater()
	{
	}
	//-------------------------------------------------------------------------------//
	void ShaderParamsUpdater::setCurrentCamera(Camera* cam)
	{
		mCamera = cam;
	}
	//-------------------------------------------------------------------------------//
	void ShaderParamsUpdater::setWorldMatrix(const Matrix4& mat)
	{
		mWorldMatrix = mat;
	}
	//-------------------------------------------------------------------------------//
	void ShaderParamsUpdater::setCurrentRenderable(Renderable* rend)
	{
		mCurrentRenderable = rend;
	}
}