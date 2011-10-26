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
	TITAN_SAFE_DELETE(mTerrain);

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
		Titan::TT_2D, 256, 256, 1,Titan::TU_DYNAMIC, Titan::PF_A8R8G8B8);

	pHeightmap->generatePerlinNoise(0.005f, 5, 0.6f);
	pHeightmap->save("terrain_noise.dds");

#else
	//Titan::TexturePtr pHeightmap = Titan::TextureMgr::getSingletonPtr()->load("terrain_noise.dds", Titan::ResourceGroupMgr::GENERAL_RESOURCE_GROUP, Titan::TT_2D, 1, Titan::PF_UNKNOWN, Titan::TU_DYNAMIC);
#endif
	//mTerrain = TITAN_NEW Titan::BaseTerrain();
	//mTerrain->create(mSceneMgr->getRootSceneNode(), pHeightmap, Titan::AABB(-500.0f, 0, -500.0f, 500.0f, 500.0f, 500.0f));



#if 0

	Titan::BaseTerrain::elevationDataVec elevationDatas;
	Titan::BaseTerrain::elevationData elevation[3];
	// grass (all elevations and slopes)
	elevation[0].minElevation = 0;
	elevation[0].maxElevation = 500;
	elevation[0].minNormalZ = -1.0f;
	elevation[0].maxNormalZ = 1.0f;
	elevation[0].strength = 1.0f;

	// rock (all elevations, steep slopes)
	elevation[1].minElevation = 0;
	elevation[1].maxElevation = 500;
	elevation[1].minNormalZ = 0.0f;
	elevation[1].maxNormalZ = 0.7f;
	elevation[1].strength = 10.0f;

	// dirt (high elevation, flat slope)
	elevation[2].minElevation = 300;
	elevation[2].maxElevation = 500;
	elevation[2].minNormalZ = 0.75f;
	elevation[2].maxNormalZ = 1.0f;
	elevation[2].strength = 20.0f;

	elevationDatas.push_back(elevation[0]);
	elevationDatas.push_back(elevation[1]);
	elevationDatas.push_back(elevation[2]);

	Titan::Image blendImage;
	blendImage.setWidth(256);
	blendImage.setHeight(256);
	blendImage.setFormat(Titan::PF_A8R8G8B8);
	blendImage.setBytesPerPixel(4);

	mTerrain->genBlendImage(blendImage, elevationDatas);
	blendImage.randomChannelNoise(3, 200, 255);

#if 1
	Titan::TexturePtr pBlendMap = Titan::TextureMgr::getSingleton().createManually(
		"blendMap", Titan::ResourceGroupMgr::GENERAL_RESOURCE_GROUP,
		Titan::TT_2D, 256, 256, 1,Titan::TU_DYNAMIC, Titan::PF_A8R8G8B8);

	pBlendMap->loadImage(blendImage);


	Titan::MaterialPtr pBlendMtrl = Titan::MaterialMgr::getSingleton().create("terrainBlend").second;
	pBlendMtrl->getPass(0)->getTextureUnit(0)->setTexture("blendMap");
	

#endif
	mTerrain->setMaterial("terrainBlend");
#endif

#if 0
	mSceneMgr->setSkybox("skybox1");
#endif

#if 1


	mManualObject = mSceneMgr->createManualObject("test");
	mManualObject->begin();
	mManualObject->setMaterial("basic", Titan::ResourceGroupMgr::GENERAL_RESOURCE_GROUP);
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

	Titan::SceneNode* node = mSceneMgr->getRootSceneNode()->createChild();
	node->rotate(Titan::Vector3::UNIT_X, Titan::Radian(3.14f * 0.25f));
	node->attachObject(mManualObject);
#endif
	float initHeight = 0.0f;
	//initHeight = mTerrain->calcMapHeight(0.0f, 0.0f);
	mCamera->setPosition(0.0f, initHeight, 0.0f);
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