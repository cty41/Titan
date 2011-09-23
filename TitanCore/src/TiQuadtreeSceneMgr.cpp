#include "TitanStableHeader.h"
#include "TiQuadtreeSceneMgr.h"
#include "TiException.h"
#include "TitanNumericTools.h"
#include "TiQuadtreeNode.h"
#include "TiQuadtree.h"

namespace Titan
{
	QuadtreeSceneMgr::QuadtreeSceneMgr(const String& name)
		:SceneMgr(name), mIsInitialized(false), mMaxDepth(0)
	{
		memset(mLevelNodes, 0, sizeof(mLevelNodes));
	}
	//-------------------------------------------------------------------------------//
	QuadtreeSceneMgr::~QuadtreeSceneMgr()
	{
		clearOctreeConfigs();
	}
	//-------------------------------------------------------------------------------//
	SceneNode* QuadtreeSceneMgr::getRootSceneNode()
	{
		if(!mRootSceneNode)
		{
			mRootSceneNode = TITAN_NEW QuadtreeNode(this, "SceneRoot");
			//attach to quadtree
			QuadtreeNode* qNode = static_cast<QuadtreeNode*>(mRootSceneNode);
			qNode->_attachToQuadTree();
		}
		
		return mRootSceneNode;
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeSceneMgr::setOctreeConfigs(const AABB& extent, int depth)
	{
		if(isInitialized())
			clearOctreeConfigs();

		if(depth < OD_MinDepth || depth > OD_MaxDepth)
		{
			TITAN_EXCEPT_INVALIDPARAMS(
				"Octree depth is invalid( 0 < depth < 10)");
			return ;
		}

		mMaxDepth = depth;
		mWorldExtents = extent.getSize();
		mWorldOffset = -extent.getMinimum();

		mWorldScale = Vector3(256.0f / mWorldExtents.x, 
			32.0f / mWorldExtents.y, 256.0f / mWorldExtents.z);

		int i ;
		for(i = 0; i < mMaxDepth; ++i)
		{
			int count = (1 << i) * (1 << i);
			mLevelNodes[i] = TITAN_NEW Quadtree[count];
		}

		for(i = 0; i < mMaxDepth; ++i)
		{
			int levelDimension = (1 << i);
			int levelIndex = 0;
			for (int z=0; z<levelDimension; ++z)
			{
				for (int x=0; x<levelDimension; ++x)
				{
					mLevelNodes[i][levelIndex].setParent(_getNodeFromLevelXZ(i-1, (x>>1),  (z>>1)));
					mLevelNodes[i][levelIndex].setChildren(
						_getNodeFromLevelXZ(i+1, (x<<1), (z<<1)), _getNodeFromLevelXZ(i+1, (x<<1)+1,(z<<1)), 
						_getNodeFromLevelXZ(i+1, (x<<1), (z<<1)+1), _getNodeFromLevelXZ(i+1, (x<<1)+1,(z<<1)+1));
					++levelIndex;
				}
			}
		}


		mIsInitialized = true;
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeSceneMgr::clearOctreeConfigs()
	{
		for(int i = 0; i < mMaxDepth; ++i)
		{
			TITAN_DELETE [] mLevelNodes[i];
			mLevelNodes[i] = NULL;
		}
		mMaxDepth = 0;
		mIsInitialized = false;
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeSceneMgr::buildQuadTreeAABB(const AABB& worldAABB, QuadTreeAABB& worldByteAABB)
	{
		worldByteAABB.convert(worldAABB, mWorldOffset, mWorldScale);
	}
	//-------------------------------------------------------------------------------//
	Quadtree* QuadtreeSceneMgr::findQuadtreeNode(const QuadTreeAABB& aabb)
	{
		int level, levelX, levelZ;
		_findTreeNodeInfo(aabb, level, levelX, levelZ);

		return _getNodeFromLevelXZ(level, levelX, levelZ);
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeSceneMgr::addOrUpdateSceneObject(QuadtreeNode* sceneNode)
	{
		QuadTreeAABB byteAABB;
		buildQuadTreeAABB(sceneNode->getAABB(), byteAABB);

		Quadtree* quadNode = findQuadtreeNode(byteAABB);

		assert(quadNode != NULL);

		//to update quadtree
		quadNode->addOrUpdateNode(sceneNode, byteAABB);
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeSceneMgr::_findTreeNodeInfo(const QuadTreeAABB& worldByteAABB, int& level, int& levelX, int& levelZ)
	{
		int xPattern = worldByteAABB.x0 ^ worldByteAABB.x1;
		int zPattern = worldByteAABB.z0 ^ worldByteAABB.z1;

		int bitPattern = maximum(xPattern, zPattern);
		int highBit = bitPattern ? highestBitSet(bitPattern)+1: 0;

		level = OD_MaxDepth-highBit-1;

		level = minimum(level, mMaxDepth-1);

		int shift = OD_MaxDepth-level-1;

		levelX = worldByteAABB.x1 >> shift;
		levelZ = worldByteAABB.z1 >> shift;
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeSceneMgr::_processVisibleObjects(Camera* cam)
	{
		QuadTreeAABB byteAABB;
	
		buildQuadTreeAABB(cam->getWorldAABB(), byteAABB);

		u32Flags heightMask = Quadtree::calcHeightMask(byteAABB.y0, byteAABB.y1);

		bool continue_search = true;
		int level = 0;

		while(level < mMaxDepth && continue_search)
		{
			int shift_count = 8-level;
			QuadTreeAABB localAABB(byteAABB.x0 >> shift_count,
				byteAABB.x1 >> shift_count,
				0,0,
				byteAABB.z0 >> shift_count,
				byteAABB.z1 >> shift_count);

			// do not continue unless a populated node is found
			continue_search = false;
			for (int z=localAABB.z0; z<=localAABB.z1; ++z)
			{
				for (int x=localAABB.x0; x<=localAABB.x1; ++x)
				{
					Quadtree* node = _getNodeFromLevelXZ(level, x, z);

					if (node->getHeightMask() & heightMask)
					{
						// a populated node has been found
						continue_search = true;

						// search all the edge cells with the full world rectangle,
						// all non-edge cells are contained within the search rect 
						// and be called with just the z extents
						if (z == localAABB.z0 
							|| z == localAABB.z1
							|| x == localAABB.x0 
							|| x == localAABB.x1)
						{

							// test all members of this node against the world rect
							node->_findVisibleObjects(mRenderQueue, heightMask, 
								cam, true);

						}
						else
						{
							// test all members of this node against 
							// the world Z extents only
							node->_findVisibleObjects(mRenderQueue, heightMask, 
										cam, false);

						}
					}
				}
			}

			// step up to the next level of the tree
			++level;
		}
	}


	//
	//QuadtreeSceneMgrFactory
	//
	QuadtreeSceneMgrFactory::QuadtreeSceneMgrFactory()
		:SceneMgrFactory()
	{
		mType = SNT_QUADTREE;
	}
	//-------------------------------------------------------------------------------//
	QuadtreeSceneMgrFactory::~QuadtreeSceneMgrFactory()
	{

	}
	//-------------------------------------------------------------------------------//
	SceneMgr*	QuadtreeSceneMgrFactory::createSceneMgr(const String& name)
	{
		return TITAN_NEW QuadtreeSceneMgr(name);
	}
}