#ifndef __TITAN_OCTREE_NODE__HH
#define __TITAN_OCTREE_NODE__HH


#include "TiPrerequisites.h"
#include "TiSceneNode.h"
#include "TiQuadtreeSceneMgr.h"
#include "TitanBitFlags.h"

namespace Titan
{
	//derive this does not mean we change the whole meaning of scenenode, but only edit some properties for octree update
	class _DllExport QuadtreeNode : public SceneNode
	{
	public:
		QuadtreeNode(QuadtreeSceneMgr* creator, const String& name);

		~QuadtreeNode();

		SceneNode*	createChild(const String& name, const Vector3& v = Vector3::ZERO,
			const Quaternion& q = Quaternion::IDENTITY);

		SceneNode*	createChild(const Vector3& v = Vector3::ZERO,
			const Quaternion& q = Quaternion::IDENTITY);

		void		removeChild(const String& name);

		void		removeChild(SceneNode* child);

		void		_findVisibleObjects(RenderQueue* queue, Camera* cam);

		Quadtree*	getAttachedQuadTree() const { return mAttachQuadNode; }

		u32Flags	getHeightMask() const { return mQuadTreeHeightMask; }

		void		_setQuadTreeData(Quadtree* attachedNode, u32Flags heightMask);

		void		_attachToQuadTree();

	protected:
		
		//this method will make the node not belong to any quadtree node, be aware of it!!
		void		detachFromQuadTree();

	protected:
		QuadtreeSceneMgr*		mCreator;
		Quadtree*				mAttachQuadNode;
		u32Flags				mQuadTreeHeightMask;

	};
}

#endif