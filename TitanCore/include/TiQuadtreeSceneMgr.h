#ifndef __TITAN_OCTREE_SCENEMGR__HH
#define __TITAN_OCTREE_SCENEMGR__HH

#include "TiPrerequisites.h"
#include "TiSceneMgr.h"
#include "TiVector3.h"
#include "TiQuadtree.h"

namespace Titan
{
	class _DllExport QuadtreeSceneMgr : public SceneMgr
	{
	public:
		enum OctreeDepth
		{
			OD_MinDepth	= 1,
			OD_MaxDepth = 9,
		};
	public:
		QuadtreeSceneMgr(const String& name);

		~QuadtreeSceneMgr();

		//which must be called before we want to use QuadtreeSceneMgr
		void			setOctreeConfigs(const AABB& extent, int depth = 4);

		void			clearOctreeConfigs();

		bool			isInitialized() const { return mIsInitialized; }

		SceneNode*		getRootSceneNode();

		void			addOrUpdateSceneObject(QuadtreeNode* sceneNode);

		void			buildQuadTreeAABB(const AABB& worldAABB, QuadTreeAABB& worldByteAABB);

		Quadtree*		findQuadtreeNode(const QuadTreeAABB& aabb);

	protected:

		virtual void	_processVisibleObjects(Camera* cam);

		void			_findTreeNodeInfo(const QuadTreeAABB& worldByteAABB, int& level, int& levelX, int& levelZ);

		Quadtree*		_getNodeFromLevelXZ(int level, int x, int z)
		{
			if (level>=0 && level< mMaxDepth)
			{
				return &mLevelNodes[level][(z<<level)+x];
			}
			return 0;
		}


	protected:
		Quadtree*	mLevelNodes[OD_MaxDepth];
		Vector3		mWorldExtents;
		Vector3		mWorldScale;
		Vector3		mWorldOffset;
		bool		mIsInitialized;
		int			mMaxDepth;
	};


	class _DllExport QuadtreeSceneMgrFactory : public SceneMgrFactory
	{
	public:
		QuadtreeSceneMgrFactory();

		virtual ~QuadtreeSceneMgrFactory();

		virtual SceneMgr*	createSceneMgr(const String& name);

	};
}


#endif