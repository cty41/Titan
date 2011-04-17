#ifndef _TITAN_SCENEMANAGER_HH
#define _TITAN_SCENEMANAGER_HH

#include "TiPrerequisites.h"

#include "TitanMatrix4.h"

namespace Titan
{
	enum SceneMgrType
	{
		SMT_GENERAL	= 0,
		SNT_QUADTREE = 1
	};
	class _DllExport SceneMgr : public GeneralAlloc
	{
	public:
		typedef list<Renderable*>::type RenderableList;
	public:
		SceneMgr(const String& name);

		virtual ~SceneMgr();

		virtual SceneNode*	getRootSceneNode();

		Camera* createCamera(const String& name);

		void	removeCamera(const String& name);

		void	_renderScene(Camera* cam, Viewport* vp);

		ManualObject*	createManualObject(const String& name);
		
		SceneObject*	createSceneObject(const String& name, const String& typeName);

		void			removeAllSceneObjects();

		SceneMgrType	getType() const { return mType; }

		void useRenderableViewProjMode(Renderable* rend);

		void resetViewProjMode();


	protected:
		SceneMgrType			mType;
		String					mName;
		Renderer*				mRelatedRenderer;
		SceneNode*				mRootSceneNode;

		typedef map<String, SceneObject*>::type SceneObjectMap;
		typedef map<String, SceneObjectMap*>::type SceneObjectCollection;
		SceneObjectCollection	mCollection;

		typedef map<String, Camera*>::type CameraMap;
		CameraMap		mCameras;
		Camera*			mCurrentCam;
		RenderQueue*	mRenderQueue;

		Matrix4			mCachedViewMatrix;
		Matrix4			mCachedProjMatrix;
		bool			mResetIdentityView;
		bool			mResetIdentityProj;

		AutoParamsSetter*	mAutoParamsSetter;
		bool			mShaderParamsOld;

	protected:
		virtual void	_updateSceneGraph();

		virtual void	_processVisibleObjects(Camera* cam);

		virtual void	_renderVisibleObjects();

		void			_renderSingleObject(Renderable* rend, const Pass* pass);

		void			_setPassSettings(Pass* pass);

		void			_updateShaderParams(const Pass* pass);
	};

	class _DllExport SceneMgrFactory : public GeneralAlloc
	{
	public:
		SceneMgrFactory();

		virtual ~SceneMgrFactory();

		virtual SceneMgr*	createSceneMgr(const String& name);

		void		destroySceneMgr(SceneMgr* mgr);

		SceneMgrType	getType() const{ return mType; }
	protected:
		SceneMgrType	mType;

	};
}
#endif