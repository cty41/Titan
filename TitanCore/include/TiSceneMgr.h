#ifndef _TITAN_SCENEMANAGER_HH
#define _TITAN_SCENEMANAGER_HH

#include "TiPrerequisites.h"
#include "TiResourceGroupMgr.h"
#include "TiMatrix4.h"

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

		void	setSkybox(const String& materialName, float distance = 3000, bool drawFirst = true, const String&  group = ResourceGroupMgr::GENERAL_RESOURCE_GROUP);

		void	disableSkybox();




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

		ManualObject*	mSkybox;
		SceneNode*		mSkyboxNode;
		bool			mSkyBoxEnable;

		enum BoxPlane
		{
			BP_FRONT = 0,
			BP_BACK = 1,
			BP_LEFT = 2,
			BP_RIGHT = 3,
			BP_UP = 4,
			BP_DOWN = 5
		};

	protected:
		virtual void	_updateSceneGraph();

		virtual void	_processVisibleObjects(Camera* cam);

		virtual void	_renderVisibleObjects();

		void			_renderSingleObject(Renderable* rend, const Pass* pass);

		void			_setPassSettings(Pass* pass);

		void			_updateShaderParams(const Pass* pass);

		void			_updateSkybox(Camera* cam);
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