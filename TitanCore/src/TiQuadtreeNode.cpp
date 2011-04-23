#include "TitanStableHeader.h"
#include "TiQuadtreeNode.h"
#include "TiQuadtree.h"
#include "TiRenderQueue.h"

namespace Titan
{
	QuadtreeNode::QuadtreeNode(QuadtreeSceneMgr* creator, const String& name)
		:SceneNode(name), mCreator(creator), mAttachQuadNode(NULL), mQuadTreeHeightMask(0)
	{
	}
	//-------------------------------------------------------------------------------//
	QuadtreeNode::~QuadtreeNode()
	{

	}
	//-------------------------------------------------------------------------------//
	SceneNode* QuadtreeNode::createChild(const String& name, const Vector3& v, const Quaternion& q )
	{
		SceneNode* node = TITAN_NEW QuadtreeNode(mCreator, name);
		node->translate(v);
		node->rotate(q);
		addChild(node);
		//attach the scenenode to quadtree node
		_attachToQuadTree();
		return node;
	}
	//-------------------------------------------------------------------------------//
	SceneNode* QuadtreeNode::createChild(const Vector3& v, const Quaternion& q)
	{
		QuadtreeNode* node = TITAN_NEW QuadtreeNode(mCreator, msAutoNamer.getAutoName());
		node->translate(v);
		node->rotate(q);
		addChild(node);

		//attach the scenenode to quadtree node
		node->_attachToQuadTree();
		return node;
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeNode::removeChild(const String& name)
	{
		SceneNodeMap::iterator it = mChildrenMap.find(name);

		if(it != mChildrenMap.end())
		{
			QuadtreeNode* qNode = static_cast<QuadtreeNode*>(it->second);
			qNode->detachFromQuadTree();
			qNode->_setParent(0);
			mChildrenMap.erase(it);
		}
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeNode::removeChild(SceneNode* child)
	{
		SceneNodeMap::iterator it = mChildrenMap.find(child->getName());

		if(it != mChildrenMap.end())
		{
			QuadtreeNode* qNode = static_cast<QuadtreeNode*>(it->second);
			qNode->detachFromQuadTree();
			qNode->_setParent(0);
			mChildrenMap.erase(it);
		}
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeNode::_findVisibleObjects(RenderQueue* queue, Camera* cam)
	{
		SceneObjectMap::iterator it = mSceneObjects.begin(), itend = mSceneObjects.end();
		while(it != itend)
		{
			it->second->_updateRenderQueue(queue, cam);
			++it;
		}
		//do not find in child nodes, because we also add child node in quadtree
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeNode::_setQuadTreeData(Quadtree* attachedNode, u32Flags heightMask)
	{
		mAttachQuadNode = attachedNode;
		mQuadTreeHeightMask = heightMask;

	}
	//-------------------------------------------------------------------------------//
	void QuadtreeNode::_attachToQuadTree()
	{
		detachFromQuadTree();
		mCreator->addOrUpdateSceneObject(this);
	}
	//-------------------------------------------------------------------------------//
	void QuadtreeNode::detachFromQuadTree()
	{
		if (mAttachQuadNode)
		{
			mAttachQuadNode->removeMember(this);
			mAttachQuadNode = NULL;
		}
	}
	//to do: change node update
}