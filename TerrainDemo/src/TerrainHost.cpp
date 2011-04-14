#include "TerrainHost.h"

TerrainHost::TerrainHost()
:BaseHost(), mManualObject(NULL),
mTerrain(NULL)
{

}
//-------------------------------------------------------------------------------//
TerrainHost::~TerrainHost()
{
	destroy();
}
//-------------------------------------------------------------------------------//
void TerrainHost::destroy()
{
	if(mTerrain)
		TITAN_DELETE mTerrain;

	BaseHost::destroy();
}
//-------------------------------------------------------------------------------//
void TerrainHost::loadResources()
{
	BaseHost::loadResources();

	mSceneMgr = mRoot->createSceneMgr("SceneMgr", Titan::SNT_QUADTREE);
	Titan::QuadtreeSceneMgr* quadMgr = static_cast<Titan::QuadtreeSceneMgr*>(mSceneMgr);
	quadMgr->setOctreeConfigs(Titan::AABB(-500.0f, 0, -500.0f,
		500.0f, 500.0f, 500.0f));


	mCamera = mSceneMgr->createCamera("testCamera");
	mWindow->addViewport(mCamera);

	mCamController = TITAN_NEW CameraController(mCamera);

#if 0
	Titan::TexturePtr pHeightmap = Titan::TextureMgr::getSingletonPtr()->createManually("heightMap1", Titan::ResourceGroupMgr::GENERAL_RESOURCE_GROUP,
		Titan::TT_2D, 128, 128, 0,Titan::TU_DYNAMIC, Titan::PF_A8R8G8B8);

	pHeightmap->generatePerlinNoise(0.01f, 5, 0.6f);

	mTerrain = TITAN_NEW Titan::ChunkTerrain();
	mTerrain->create(mSceneMgr->getRootSceneNode(), pHeightmap, Titan::AABB(-500.0f, 0, -500.0f, 500.0f, 500.0f, 500.0f));

#endif

#if 0
	Titan::ShaderEffectPtr pEffect = Titan::ShaderEffectMgr::getSingletonPtr()->loadManually("DefaultMesh.fx", Titan::ResourceGroupMgr::GENERAL_RESOURCE_GROUP);


	mManualObject = mSceneMgr->createManualObject("test");
	mManualObject->begin();
	mManualObject->position(-1.0f,-1.0f,-1.0f);			//0
	mManualObject->position(-1.0f, 1.0f,-1.0f);			//1
	mManualObject->position( 1.0f, 1.0f,-1.0f);			//2
	mManualObject->position( 1.0f,-1.0f,-1.0f);			//3
	mManualObject->position(-1.0f,-1.0f, 1.0f);			//4
	mManualObject->position(-1.0f, 1.0f, 1.0f);			//5
	mManualObject->position( 1.0f, 1.0f, 1.0f);			//6
	mManualObject->position( 1.0f,-1.0f, 1.0f);			//7
	mManualObject->quad(0, 1, 2, 3);
	mManualObject->quad(4, 7, 6, 5);
	mManualObject->quad(4, 5, 1, 0);
	mManualObject->quad(3, 2, 6, 7);
	mManualObject->quad(1, 5, 6, 2);
	mManualObject->quad(4, 0, 3, 7);
	mManualObject->end();
	mManualObject->setShaderEffect(pEffect);

	Titan::SceneNode* node = mSceneMgr->getRootSceneNode()->createChild();
	node->rotate(Titan::Vector3::UNIT_X, Titan::Radian(3.14f * 0.25f));
	node->attachObject(mManualObject);
#endif

	mCamera->setPosition(0.0f, 0.0f, 10.0f);
}
//-------------------------------------------------------------------------------//
void TerrainHost::render(float frameTime)
{

	if(mManualObject)
	{
		Titan::SceneNode* node = mManualObject->getAttachedNode();
		if(node)
		{
			// incremement y-rotation angle each frame
			node->rotate(Titan::Vector3::UNIT_Y, Titan::Radian(frameTime * 0.001f));
		}
	}
	BaseHost::render(frameTime);
}
//-------------------------------------------------------------------------------//
void TerrainHost::updateLogic(float frameTime)
{
	BaseHost::updateLogic(frameTime);
}