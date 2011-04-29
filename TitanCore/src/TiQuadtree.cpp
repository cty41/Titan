#include "TitanStableHeader.h"
#include "TiQuadtree.h"
#include "TitanAABB.h"
#include "TiQuadtreeNode.h"
#include "TiMath.h"

namespace Titan
{
	Quadtree::Quadtree(Quadtree* parent)
		: mParent(parent), mHeightLocalMask(0), mHeightMask(0)
	{
		memset(mChildren, 0, sizeof(mChildren));
	}
	//-------------------------------------------------------------------------------//
	Quadtree::~Quadtree()
	{

	}
	//-------------------------------------------------------------------------------//
	void Quadtree::setParent(Quadtree* parent)
	{
		assert(mParent == NULL);
		mParent = parent;
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::setChildren(Quadtree* child1, Quadtree* child2, Quadtree* child3, Quadtree* child4)
	{
		mChildren[0] = child1;
		mChildren[1] = child2;
		mChildren[2] = child3;
		mChildren[3] = child4;
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::addOrUpdateNode(QuadtreeNode* member, const QuadTreeAABB& aabb)
	{
		u32Flags heightMask = calcHeightMask(aabb.y0, aabb.y1);

		//to add some judges
		if(member->getAttachedQuadTree() != this)
		{
			if(member->getAttachedQuadTree())
			{
				member->getAttachedQuadTree()->removeMember(member);
			}

			mNodeMap.insert(SceneNodeMap::value_type(member->getName(), member));

			//update height mask
			mHeightMask.setFlags(heightMask);
			mHeightLocalMask.setFlags(heightMask);

			if (mParent)
			{
				//notify the parent
				mParent->descendantMemberAdded(mHeightMask);
			}
		}
		else
		{
			//update height mask for all members
			rebuildLocalHeightMask();
		}

		member->_setQuadTreeData(this, heightMask);
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::removeMember(QuadtreeNode* member)
	{
		if(member->getAttachedQuadTree() != this)
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"Error removing quad tree member",
				"Quadtree::removeMember");
		}
		else
		{
			SceneNodeMap::iterator it = mNodeMap.find(member->getName());
			if(it != mNodeMap.end())
			{
				mNodeMap.erase(it);
			}

			rebuildLocalHeightMask();

			if(mParent)
				mParent->descendantMemberRemoved();
		}
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::rebuildLocalHeightMask()
	{
		mHeightLocalMask.clear();

		SceneNodeMap::iterator it = mNodeMap.begin(), itend = mNodeMap.end();
		while(it != itend)
		{
			mHeightLocalMask.setFlags(it->second->getHeightMask());
			++it;
		}
	
		rebuildHeightMask();
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::rebuildHeightMask()
	{
		mHeightMask = mHeightLocalMask;

		for(uint i = 0; i < 4; ++i)
		{
			if(mChildren[i])
				mHeightMask.setFlags(mChildren[i]->getHeightMask());
		}
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::descendantMemberAdded(u32Flags heightMask)
	{
		mHeightMask.setFlags(heightMask);

		if(mParent)
			mParent->descendantMemberAdded(heightMask);
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::descendantMemberRemoved()
	{
		rebuildHeightMask();
		
		if(mParent)
			mParent->descendantMemberRemoved();
	}
	//-------------------------------------------------------------------------------//
	void Quadtree::_findVisibleObjects(RenderQueue* queue, u32Flags heightMask, Camera* cam, bool state)
	{
		AABB result;
		if(mHeightLocalMask.testAny(heightMask))
		{
			SceneNodeMap::iterator it = mNodeMap.begin(), itend = mNodeMap.end();
			while(it != itend)
			{
				QuadtreeNode* node = it->second;
				if(node->getHeightMask().testAny(heightMask))
				{
					if(state)
					{
						if(Math::intersects(cam->getWorldAABB(), node->getAABB(), result))
						{
							if(cam->isVisible(node->getAABB()))
								node->_findVisibleObjects(queue, cam);
						}
					}
					else
					{
						if(cam->isVisible(node->getAABB()))
							node->_findVisibleObjects(queue, cam);
					}

				}
				++it;
			}
		}
	}
	//-------------------------------------------------------------------------------//



}