#ifndef __TITAN_AUTOPARAMSSETTER__HH
#define __TITAN_AUTOPARAMSSETTER__HH

#include "TiPrerequisites.h"

namespace Titan
{
	class _DllExport AutoParamsSetter : public GeneralAlloc
	{
	public:
		AutoParamsSetter();

		~AutoParamsSetter(){}

		void	setCurrentRenderable(Renderable* rend);

		void	setWorldMatrix(const Matrix4& mat);

		void	setCurrentCamera(Camera* cam);

		void	setCurrentPass(Pass* pass);

		const Matrix4&	getWorldMatrix();
		const Matrix4&	getViewMatrix();
		const Matrix4&	getProjMatrix();
		const Matrix4&	getViewProjMatrix();
		const Matrix4&	getWorldViewMatrix();
		const Matrix4&	getWorldViewProjMatrix();

	protected:
		Matrix4	mWorldMatrix;
		Matrix4 mViewMatrix;
		Matrix4 mProjMatrix;
		Matrix4	mWorldViewMatrix;
		Matrix4 mViewProjMatrix;
		Matrix4 mWorldViewProjMatrix;


		bool mWorldMatrixDirty;
		bool mViewMatrixDirty;
		bool mProjMatrixDirty;
		bool mWorldViewMatrixDirty;
		bool mViewProjMatrixDirty;
		bool mWorldViewProjMatrixDirty;

		Renderable*	mCurrentRenderable;
		Camera*		mCurrentCam;
		Pass*		mCurrentPass;

	};
}
#endif