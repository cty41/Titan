#include "TitanStableHeader.h"
#include "TiSceneMgr.h"
#include "TiRoot.h"
#include "TiManualObject.h"
#include "TiViewport.h"
#include "TiLogMgr.h"
#include "TiRenderQueue.h"
#include "TiRenderQueueGroup.h"
#include "TiOverlayMgr.h"
#include "TiPass.h"
#include "TiAutoParamsSetter.h"
#include "TiMaterialMgr.h"

namespace Titan
{
	SceneMgr::SceneMgr(const String& name)
		: mType(SMT_GENERAL), mName(name),
		mRootSceneNode(0), mResetIdentityView(false), mResetIdentityProj(false),
		mCachedViewMatrix(Matrix4::IDENTITY), mCachedProjMatrix(Matrix4::IDENTITY),
		mCurrentCam(NULL), mShaderParamsOld(true), mSkybox(NULL), mSkyboxNode(NULL), mSkyBoxEnable(false)
	{
		mRelatedRenderer = Root::getSingletonPtr()->getActiveRenderer();

		mRenderQueue = TITAN_NEW RenderQueue();

		mAutoParamsSetter = TITAN_NEW AutoParamsSetter();
	}
	//-------------------------------------------------------------------------------//
	SceneMgr::~SceneMgr()
	{
		if(mRootSceneNode)
			TITAN_DELETE mRootSceneNode;
		removeAllSceneObjects();

		if (mSkyboxNode)
			TITAN_DELETE mSkyboxNode;
		if(mSkybox)
			TITAN_DELETE mSkybox;

		//remove later
		if(mCurrentCam)
			TITAN_DELETE mCurrentCam;

		TITAN_DELETE(mRenderQueue);
	}	
	//-------------------------------------------------------------------------------//
	SceneNode*	SceneMgr::getRootSceneNode()
	{
		if(!mRootSceneNode)
		{
			mRootSceneNode = TITAN_NEW SceneNode("SceneRoot");
		
		}
		return mRootSceneNode;
	}
	//-------------------------------------------------------------------------------//
	Camera*	SceneMgr::createCamera(const String& name)
	{
		CameraMap::iterator it = mCameras.find(name);
		if(it != mCameras.end())
		{
			TITAN_EXCEPT_INVALIDPARAMS("This name has been used for other camera");
			return 0;
		}

		Camera* cam = TITAN_NEW Camera(name, this);
		mCameras[name] = cam;
	
		return cam;
	}
	//-------------------------------------------------------------------------------//
	void SceneMgr::removeCamera(const String& name)
	{
		CameraMap::iterator it = mCameras.find(name);
		if(it != mCameras.end())
		{
			mCameras.erase(it);
		}
		else
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"This name has not been used for any cameras");
		}
	}
	//-------------------------------------------------------------------------------//
	void SceneMgr::_updateSceneGraph()
	{
		if(mRootSceneNode)
		{
			mRootSceneNode->_update();
		}
	}
	//-------------------------------------------------------------------------------//
	void SceneMgr::_processVisibleObjects(Camera* cam)
	{
		mRootSceneNode->_findVisibleObjects(cam, mRenderQueue);
	}
	//-------------------------------------------------------------------------------//
	void SceneMgr::_renderVisibleObjects()
	{
		RenderQueue::RenderQueueGroupMapIterator git = mRenderQueue->getGroupIterator();
		while(git.hasMoreElements())
		{
			//mRelatedRenderer->_setSceneBlending(SBF_ONE, SBF_ZERO);
			RenderQueueGroup::RenderQueueEntryVecIterator oit = git.peekNextValue()->getOpaqueEntryIterator();
			while (oit.hasMoreElements())
			{
				_setPassSettings(oit.peekNext().rendPass);
				_renderSingleObject(oit.peekNext().renderable, oit.peekNext().rendPass);
				oit.next();
			}


			//mRelatedRenderer->_setSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
			RenderQueueGroup::RenderQueueEntryVecIterator tit = git.peekNextValue()->getSortedTransparentEntryIterator();
			while (tit.hasMoreElements())
			{
				_setPassSettings(tit.peekNext().rendPass);
				_renderSingleObject(tit.peekNext().renderable, tit.peekNext().rendPass);
				tit.next();
			}
			git.next();
		}
	}
	//-------------------------------------------------------------------------------//
	//------------------------------------------------------------------------------//
	void SceneMgr::_updateSkybox(Camera* cam)
	{
		if (mSkyBoxEnable)
		{
			mSkyboxNode->setPosition(cam->getPosition());
		}
		if(mSkyBoxEnable)
			mSkybox->_updateRenderQueue(mRenderQueue, cam);
	}

	//////////////////////////////////////////////////////////////////////////
	//MAIN RENDER FUNCTION
	//////////////////////////////////////////////////////////////////////////	
	void SceneMgr::_renderScene(Camera* cam, Viewport* vp)
	{
		mCurrentCam = cam;
		mRenderQueue->setCurrentCam(cam);

		_updateSceneGraph();

		_updateSkybox(cam);

		_processVisibleObjects(cam);

		OverlayMgr::getSingleton().updateRenderQueue(mRenderQueue, vp);

		mRelatedRenderer->_setViewport(vp);
		mRenderQueue->sort();

		mAutoParamsSetter->setCurrentCamera(cam);


		//begin geometry count
		mRelatedRenderer->_beginGeometryCount();
		//clear viewport
		mRelatedRenderer->_clearFrameBuffer(vp->getClearBuffers(), vp->getBackgroundColor());
		//begin frame
		mRelatedRenderer->_beginFrame();

		mCachedViewMatrix = cam->getViewMatrix();
		mRelatedRenderer->_setViewMatrix(mCachedViewMatrix);

		mCachedProjMatrix = cam->getProjMatrixRS();
		mRelatedRenderer->_setProjMatrix(mCachedProjMatrix);

		_renderVisibleObjects();

		mRelatedRenderer->_endFrame();

		//clean the render queue
		mRenderQueue->clear();

		// Notify camera of vis faces
		cam->_notifyRenderedFaces(mRelatedRenderer->_getFaceCount());

		// Notify camera of vis batches
		cam->_notifyRenderedBatches(mRelatedRenderer->_getBatchCount());
	}
	//----------------------------------------------------------------------------//
	void SceneMgr::_setPassSettings(Pass* pass)
	{
		
		mAutoParamsSetter->setCurrentPass(pass);

		if (pass->hasVertexShader())
		{
			mRelatedRenderer->_setShader(pass->getVertexShaderUnit()->getShader().getPointer());
		}
		else
		{
			if(mRelatedRenderer->isShaderSet(ST_VERTEX_SHADER))
				mRelatedRenderer->_clearShader(ST_VERTEX_SHADER);
		}

		if (pass->hasPixelShader())
		{
			mRelatedRenderer->_setShader(pass->getPixelShaderUnit()->getShader().getPointer());
		}
		else
		{
			if(mRelatedRenderer->isShaderSet(ST_PIXEL_SHADER))
				mRelatedRenderer->_clearShader(ST_PIXEL_SHADER);
		}

		//alpha blend settings
		mRelatedRenderer->_setSceneBlending(pass->getSrcBlendFactor(), pass->getDstBlendFactor(), pass->getSceneBlendOperation());

		size_t texIdx = 0;
		Pass::TextureUnitVecIterator tit = pass->getTextureUnitVecIterator();
		while (tit.hasMoreElements())
		{
			mRelatedRenderer->_setTextureUnit(texIdx, *tit.current());
			tit.next();
			++texIdx;
		}

		//depth settings
		mRelatedRenderer->_setDepthWrite(pass->isDepthWritable());
		if(pass->isDepthCheck())
		{
			mRelatedRenderer->_setDepthCheck(pass->isDepthCheck());
			mRelatedRenderer->_setDepthFuntion(pass->getDepthFunc());
		}

		mRelatedRenderer->_setPolygonMode(pass->getPolygonMode());
		mRelatedRenderer->_setCullingMode(pass->getCullMode());

		mShaderParamsOld = true;
	}
	//----------------------------------------------------------------------------//	
	void SceneMgr::_renderSingleObject(Renderable* rend, const Pass* pass)
	{
		Matrix4 transformMat;
		rend->getTransformMat(&transformMat);
		//programmable
		if(pass->isProgrammable())
		{
			mAutoParamsSetter->setCurrentRenderable(rend);
			mAutoParamsSetter->setWorldMatrix(transformMat);

			_updateShaderParams(pass);
		}
		else 
		{
			//fixed pipeline  will be removed in later version
			mRelatedRenderer->_setWorldMatrix(transformMat);
		}
		useRenderableViewProjMode(rend);

		if (pass->IsLightEnable())
		{
			//to do in later version
		}
		else
		{
			mRelatedRenderer->_setLightEnable(false);
		}

		RenderData* rd;
		rd = rend->getRenderData();
		mRelatedRenderer->_render(*rd);

		resetViewProjMode();
	}
	//------------------------------------------------------------------------------//
	void SceneMgr::_updateShaderParams(const Pass* pass)
	{
		if (pass->isProgrammable())
		{
			if(!mShaderParamsOld)
				return;

			pass->updateAutoParams(mAutoParamsSetter);

			if(pass->hasVertexShader())
			{
				mRelatedRenderer->_setShaderParams(ST_VERTEX_SHADER, pass->getVertexShaderParams());
			}

			if (pass->hasPixelShader())
			{
				mRelatedRenderer->_setShaderParams(ST_PIXEL_SHADER, pass->getPixelShaderParams());
			}

			mShaderParamsOld = false;
		}
	}
	//-------------------------------------------------------------------------------//
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
				TITAN_EXCEPT_INVALIDPARAMS(
					"The SceneObject called" + name + "has existed in type " + typeName);
			}
		}

		return object;
	}
	//-------------------------------------------------------------------------------//
	ManualObject* SceneMgr::createManualObject(const String& name)
	{
		return static_cast<ManualObject*>(createSceneObject(name, "ManualObject"));
	}
	//-------------------------------------------------------------------------------//
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
	//-------------------------------------------------------------------------------//
	void SceneMgr::useRenderableViewProjMode(Renderable* rend)
	{
		if(rend->IsUseIdentityView())
		{
			mRelatedRenderer->_setViewMatrix(Matrix4::IDENTITY);
			mResetIdentityView = true;
		}

		if (rend->IsUseIdentityProj())
		{
			Matrix4 mat;
			mRelatedRenderer->_convertProjMatrix(Matrix4::IDENTITY, mat, false);
			mRelatedRenderer->_setProjMatrix(mat);
			mResetIdentityProj = true;
		}
	}
	//-------------------------------------------------------------------------------//
	void SceneMgr::resetViewProjMode()
	{
		if(mResetIdentityView)
		{
			mRelatedRenderer->_setViewMatrix(mCachedViewMatrix);
			mResetIdentityView = false;
		}

		if(mResetIdentityProj)
		{
			mRelatedRenderer->_setProjMatrix(mCachedProjMatrix);
			mResetIdentityProj = false;
		}
	}
	//------------------------------------------------------------------------------//
	void SceneMgr::setSkybox(const String& materialName, float distance, bool drawFirst , const String& group)
	{
		MaterialPtr mp = MaterialMgr::getSingleton().load(materialName, group);
		mp->setDepthWrite(false);
		uint skyboxQueueId = drawFirst? RGT_Sky_Early : RGT_Sky_Late;
	
		if(!mSkybox)
		{
			mSkybox = createManualObject("Skybox");
			mSkyboxNode = TITAN_NEW SceneNode("SkyboxNode");
			mSkyboxNode->attachObject(mSkybox);
		}
		else
		{
			mSkybox->clear();
		}

		
		mSkybox->setRenderQueueID(skyboxQueueId);

		mSkybox->begin();
		mSkybox->setMaterial(materialName, group);

		for (uint16 i = 0; i < 6; ++i)
		{
			Plane plane;
			String meshName;
			Vector3 middle;
			Vector3 up, right;

			switch(i)
			{
			case BP_FRONT:
				middle = Vector3(0, 0, -distance);
				up = Vector3::UNIT_Y * distance;
				right = Vector3::UNIT_X * distance;
				break;
			case BP_BACK:
				middle = Vector3(0, 0, distance);
				up = Vector3::UNIT_Y * distance;
				right = Vector3::NEGATIVE_UNIT_X * distance;
				break;
			case BP_LEFT:
				middle = Vector3(-distance, 0, 0);
				up = Vector3::UNIT_Y * distance;
				right = Vector3::NEGATIVE_UNIT_Z * distance;
				break;
			case BP_RIGHT:
				middle = Vector3(distance, 0, 0);
				up = Vector3::UNIT_Y * distance;
				right = Vector3::UNIT_Z * distance;
				break;
			case BP_UP:
				middle = Vector3(0, distance, 0);
				up = Vector3::UNIT_Z * distance;
				right = Vector3::UNIT_X * distance;
				break;
			case BP_DOWN:
				middle = Vector3(0, -distance, 0);
				up = Vector3::NEGATIVE_UNIT_Z * distance;
				right = Vector3::UNIT_X * distance;
				break;
			}

			// 3D cubic texture 
			// Note UVs mirrored front/back
			// I could save a few vertices here by sharing the corners
			// since 3D coords will function correctly but it's really not worth
			// making the code more complicated for the sake of 16 verts
			// top left
			Vector3 pos;
			pos = middle + up - right;
			mSkybox->position(pos);
			mSkybox->texCoord(pos.normalisedCopy() * Vector3(1,1,-1));
			// bottom left
			pos = middle - up - right;
			mSkybox->position(pos);
			mSkybox->texCoord(pos.normalisedCopy() * Vector3(1,1,-1));
			// bottom right
			pos = middle - up + right;
			mSkybox->position(pos);
			mSkybox->texCoord(pos.normalisedCopy() * Vector3(1,1,-1));
			// top right
			pos = middle + up + right;
			mSkybox->position(pos);
			mSkybox->texCoord(pos.normalisedCopy() * Vector3(1,1,-1));

			uint16 base = i * 4;
			mSkybox->quad(base, base+1, base+2, base+3);
		}
		mSkybox->end();
		mSkyBoxEnable = true;
	}
	//------------------------------------------------------------------------------//
	void SceneMgr::disableSkybox()
	{
		mSkyBoxEnable = false;
	}




	//
	//SceneMgrFactory
	//
	SceneMgrFactory::SceneMgrFactory()
		:mType(SMT_GENERAL)
	{
	}
	//-------------------------------------------------------------------------------//
	SceneMgrFactory::~SceneMgrFactory()
	{

	}
	//-------------------------------------------------------------------------------//
	SceneMgr*	SceneMgrFactory::createSceneMgr(const String& name)
	{
		return TITAN_NEW SceneMgr(name);
	}
	//-------------------------------------------------------------------------------//
	void SceneMgrFactory::destroySceneMgr(SceneMgr* mgr)
	{
		TITAN_DELETE mgr;
	}

}