#include "TitanStableHeader.h"
#include "SceneMgr.h"
#include "Root.h"
#include "ManualObject.h"
#include "Viewport.h"
#include "TitanShaderParamsUpdater.h"

namespace Titan
{
	SceneMgr::SceneMgr(const String& name)
		: mType(SMT_GENERAL), mName(name),
		mRootSceneNode(0)
	{
		mRelatedRenderer = Root::getSingletonPtr()->getActiveRenderer();

		mShaderParamsUpdater = TITAN_NEW ShaderParamsUpdater();
	}
	//-------------------------------------------------------------//
	SceneMgr::~SceneMgr()
	{
		if(mRootSceneNode)
			TITAN_DELETE mRootSceneNode;
		removeAllSceneObjects();

		TITAN_DELETE(mShaderParamsUpdater);
	}	
	//-------------------------------------------------------------//
	SceneNode*	SceneMgr::getRootSceneNode()
	{
		if(!mRootSceneNode)
		{
			mRootSceneNode = TITAN_NEW SceneNode("SceneRoot");
		
		}
		return mRootSceneNode;
	}
	//-------------------------------------------------------------//
	Camera*	SceneMgr::createCamera(const String& name)
	{
		CameraMap::iterator it = mCameras.find(name);
		if(it != mCameras.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"This name has been used for other camera",
				"SceneMgr::createCamera");
			return 0;
		}

		Camera* cam = TITAN_NEW Camera(name, this);
		mCameras[name] = cam;
	
		return cam;
	}
	//-------------------------------------------------------------//
	void SceneMgr::removeCamera(const String& name)
	{
		CameraMap::iterator it = mCameras.find(name);
		if(it != mCameras.end())
		{
			mCameras.erase(it);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"This name has not been used for any cameras",
				"SceneMgr::removeCamera");
		}
	}
	//-------------------------------------------------------------//
	void SceneMgr::_updateSceneGraph()
	{
		mRootSceneNode->_update();
	}
	//-------------------------------------------------------------//
	void SceneMgr::_processVisibleObjects(Camera* cam)
	{
		_buildRenderUnitQueue(mRootSceneNode);
	}
	//-------------------------------------------------------------//
	void SceneMgr::_buildRenderUnitQueue(SceneNode* node)
	{
		SceneNode::ConstChildIterator it = node->getConstChildIterator();
		while (it.hasMoreElements())
		{
			_buildRenderUnitQueue(it.getNext());
		}
		SceneNode::ConstObjectIterator objIt = node->getAttachedConstObjectIterator();
		while (objIt.hasMoreElements())
		{
			objIt.getNext()->_updateRenderableList(mRenderableList);
		}

	}
	//-------------------------------------------------------------//
	void SceneMgr::_renderOpaqueObjects()
	{
		RenderableList::iterator it = mRenderableList.begin(),
			itend = mRenderableList.end();
		while (it != itend)
		{
			_renderSingleObject(*it++);
		}
	}
	//-------------------------------------------------------------//
	void SceneMgr::_renderScene(Camera* cam, Viewport* vp)
	{
		_updateSceneGraph();

		_processVisibleObjects(cam);

		mRelatedRenderer->_clearFrameBuffer(vp->getClearBuffers(), vp->getBackgroundColor());
		mRelatedRenderer->_beginFrame();

		mRelatedRenderer->_setViewMatrix(cam->getViewMatrix());

		mRelatedRenderer->_setProjMatrix(cam->getProjMatrixRS());

		mShaderParamsUpdater->setCurrentCamera(cam);

		_renderOpaqueObjects();

		mRelatedRenderer->_endFrame();

		mRenderableList.clear();
	}
	//-------------------------------------------------------------//
	void SceneMgr::_renderSingleObject(Renderable* rend)
	{
		Matrix4 transformMat;
		rend->getTransformMat(&transformMat);

		//programmable shader
		if(rend->hasShader())
		{
			ShaderEffectPtr effectPtr = rend->getShaderEffect();
			effectPtr->begin();
			mShaderParamsUpdater->setCurrentRenderable(rend);
			mShaderParamsUpdater->setWorldMatrix(transformMat);
			_updateShaderParams(effectPtr);

			RenderData rd;
			rend->getRenderData(rd);
			mRelatedRenderer->_render(rd);

			effectPtr->end();
		}
		else
		{
			//fixed pipeline
			mRelatedRenderer->_setWorldMatrix(transformMat);

			RenderData rd;
			rend->getRenderData(rd);
			mRelatedRenderer->_render(rd);
		}
	}
	//-------------------------------------------------------------//
	void SceneMgr::_updateShaderParams(ShaderEffectPtr effect)
	{
		effect->updateParams(mShaderParamsUpdater);
	}
	//-------------------------------------------------------------//
	SceneObject* SceneMgr::createSceneObject(const String& name, const String& typeName)
	{
		SceneObjectFactory* factory = Root::getSingletonPtr()->getSceneObjectFactory(typeName);
		SceneObject* object = 0;
		if(factory)
		{
			SceneObjectCollection::iterator it = mCollection.find(typeName);
			SceneObjectMap* objectMap;
			if(it == mCollection.begin())
			{
				objectMap = TITAN_NEW_T(SceneObjectMap, MEMCATEGORY_GENERAL)();
				mCollection.insert(SceneObjectCollection::value_type(typeName, objectMap));
			}
			else
			{
				objectMap = it->second;
			}

			SceneObjectMap::iterator i = objectMap->find(name);
			if(i == objectMap->end())
			{
				object = factory->createInstance(name);
				(*objectMap)[name] = object;
			}
			else
			{
				TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
					"The SceneObject called" + name + "has existed in type " + typeName,
					"SceneMgr::createSceneObject");
			}
		}

		return object;
	}
	//-------------------------------------------------------------//
	ManualObject* SceneMgr::createManualObject(const String& name)
	{
		return static_cast<ManualObject*>(createSceneObject(name, "ManualObject"));
	}
	//-------------------------------------------------------------//
	void SceneMgr::removeAllSceneObjects()
	{
		SceneObjectCollection::iterator it = mCollection.begin(),
			itend = mCollection.end();
		SceneObjectMap* objectMap = 0;
		while(it != itend)
		{
			objectMap = it->second;
			SceneObjectMap::iterator i = objectMap->begin(),
				iend = objectMap->end();
			while (i != iend)
			{
				TITAN_DELETE i->second;
				++i;
			}
			objectMap->clear();
			TITAN_DELETE_T(objectMap, SceneObjectMap, MEMCATEGORY_GENERAL);
			++it;
		}
		mCollection.clear();

	}

	//
	//SceneMgrFactory
	//
	SceneMgrFactory::SceneMgrFactory()
		:mType(SMT_GENERAL)
	{
	}
	//-------------------------------------------------------------//
	SceneMgrFactory::~SceneMgrFactory()
	{

	}
	//-------------------------------------------------------------//
	SceneMgr*	SceneMgrFactory::createSceneMgr(const String& name)
	{
		return TITAN_NEW SceneMgr(name);
	}
	//-------------------------------------------------------------//
	void SceneMgrFactory::destroySceneMgr(SceneMgr* mgr)
	{
		TITAN_DELETE mgr;
	}

}