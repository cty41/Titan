#ifndef _TITAN_SCENEMANAGER_HH
#define _TITAN_SCENEMANAGER_HH

#include "TitanPrerequisites.h"
#include "TitanShaderEffect.h"

namespace Titan
{
	enum SceneMgrType
	{
		SMT_GENERAL	= 0,
		SNT_CHUNK_TERRAIN = 1
	};
	class _DllExport SceneMgr : public GeneralAlloc
	{
	public:
		typedef list<Renderable*>::type RenderableList;
	public:
		SceneMgr(const String& name);

		virtual ~SceneMgr();

		SceneNode*	getRootSceneNode();

		Camera* createCamera(const String& name);

		void	removeCamera(const String& name);

		void	_renderScene(Camera* cam, Viewport* vp);

		ManualObject*	createManualObject(const String& name);
		
		SceneObject*	createSceneObject(const String& name, const String& typeName);

		void			removeAllSceneObjects();

		SceneMgrType	getType() const { return mType; }


	protected:
		SceneMgrType			mType;
		String					mName;
		Renderer*				mRelatedRenderer;
		SceneNode*				mRootSceneNode;
		ShaderParamsUpdater*	mShaderParamsUpdater;	

		typedef map<String, SceneObject*>::type SceneObjectMap;
		typedef map<String, SceneObjectMap*>::type SceneObjectCollection;
		SceneObjectCollection	mCollection;

		typedef map<String, Camera*>::type CameraMap;
		CameraMap		mCameras;

		RenderableList mRenderableList;

	protected:
		virtual void	_updateSceneGraph();

		virtual void	_processVisibleObjects(Camera* cam);

		virtual void	_renderOpaqueObjects();

		virtual void	_buildRenderUnitQueue(SceneNode* node);

		virtual void	_renderSingleObject(Renderable* rend);

		virtual void	_updateShaderParams(ShaderEffectPtr effect);
	};

	class _DllExport SceneMgrFactory : public GeneralAlloc
	{
	public:
		SceneMgrFactory();

		virtual ~SceneMgrFactory();

		SceneMgr*	createSceneMgr(const String& name);

		void		destroySceneMgr(SceneMgr* mgr);

		SceneMgrType	getType() const{ return mType; }
	protected:
		SceneMgrType	mType;

	};
}
#endif