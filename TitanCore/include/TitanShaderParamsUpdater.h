#ifndef _TITAN_SHADER_PARAMSUPDATER__HH
#define _TITAN_SHADER_PARAMSUPDATER__HH


#include "TitanPrerequisites.h"
#include "Camera.h"
#include "TitanMatrix4.h"

namespace Titan
{
	class _DllExport ShaderParamsUpdater : public GeneralAlloc
	{
	public:
		ShaderParamsUpdater();

		~ShaderParamsUpdater();


		void	setCurrentCamera(Camera* cam);

		void	setWorldMatrix(const Matrix4& mat);

		void	setCurrentRenderable(Renderable* rend);

		const Matrix4&	getWorldMatrix() const { return mWorldMatrix; }

		const Matrix4&	getViewMatrix() const { return mCamera->getViewMatrix();}

		const Matrix4&	getProjMatrix() const { return mCamera->getProjMatrixRS(); }


	protected:

		Camera*			mCamera;
		Renderable*		mCurrentRenderable;
		Matrix4			mWorldMatrix;
	};
}


#endif