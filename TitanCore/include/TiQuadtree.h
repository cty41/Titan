#ifndef __TITAN__OCTREE__HH
#define __TITAN__OCTREE__HH

#include "TiPrerequisites.h"
#include "TitanBitFlags.h"
#include "TitanAABB.h"
#include "TitanMath.h"
#include "TitanNumericTools.h"
#include "TiSceneMgr.h"

namespace Titan
{
	class _DllExport QuadTreeAABB : public GeneralAlloc
	{
	public:
		int x0, y0, z0;
		int x1, y1, z1;

		QuadTreeAABB(){};

		~QuadTreeAABB(){};

		QuadTreeAABB(const QuadTreeAABB& src)
		:x0(src.x0),x1(src.x1),y0(src.y0),y1(src.y1),z0(src.z0),z1(src.z1)
		{};

		QuadTreeAABB(int _x0, int _x1,int _y0, int _y1,int _z0,int _z1)
		:x0(_x0),x1(_x1),y0(_y0),y1(_y1),z0(_z0),z1(_z1)
		{};

		void convert(const AABB& world, const Vector3& offset, const Vector3& scale)
		{
			AABB converted(world);

			// reposition and scale world coordinates to quad tree coordinates
			converted += offset;
			converted *= scale;

			// reduce by a tiny amount to handle tiled data
			converted.getMaximum().x -= 0.01f;
			converted.getMaximum().y -= 0.01f;
			converted.getMaximum().z -= 0.01f;

			converted.getMaximum().x = maximum(converted.getMaximum().x, converted.getMinimum().x);
			converted.getMaximum().y = maximum(converted.getMaximum().y, converted.getMinimum().y);
			converted.getMaximum().z = maximum(converted.getMaximum().z, converted.getMinimum().z);

			// convert to integer values, taking the floor of each real
			x0 = realToInt32_floor(converted.getMinimum().x);
			x1 = realToInt32_floor(converted.getMaximum().x);
			y0 = realToInt32_floor(converted.getMinimum().y);
			y1 = realToInt32_floor(converted.getMaximum().y);
			z0 = realToInt32_floor(converted.getMinimum().z);
			z1 = realToInt32_floor(converted.getMaximum().z);

			// we must be positive
			x0 = clamp(x0, 0, 254);
			y0 = clamp(y0, 0, 30);
			z0 = clamp(z0, 0, 254);

			// we must be at least one unit large
			x1 = clamp(x1, x0+1, 255);
			y1 = clamp(y1, y0+1, 31);
			z1 = clamp(z1, z0+1, 255);

		}
	};


	class _DllExport Quadtree : public GeneralAlloc
	{
	public:
		Quadtree(Quadtree* parent = NULL);

		~Quadtree();

		void		setParent(Quadtree* parent);

		void		setChildren(Quadtree* child1, Quadtree* child2, Quadtree* child3, Quadtree* child4);

		void		addOrUpdateNode(QuadtreeNode* member, const QuadTreeAABB& aabb);

		void		removeMember(QuadtreeNode* member);

		bool		empty() const { return(mHeightMask.value == 0); }

		u32Flags	getHeightMask() const { return mHeightMask; }

		u32Flags	getLocalHeightMask() const { return mHeightLocalMask; }

		void		_findVisibleObjects(RenderQueue* queue, 
										u32Flags heightMask,
										Camera* cam, 
										bool state);

	protected:
		void		descendantMemberAdded(u32Flags heightMask);

		void		descendantMemberRemoved();

		void		rebuildLocalHeightMask();

		void		rebuildHeightMask();

	public:
		typedef map<String, QuadtreeNode*>::type SceneNodeMap;

	protected:
		Quadtree*			mChildren[4];
		Quadtree*			mParent;
		SceneNodeMap		mNodeMap;

		u32Flags			mHeightLocalMask;
		u32Flags			mHeightMask;

	public:

		static uint32 calcHeightMask(uint8 min, uint8 max)
		{
			uint32 high = (1<<max);
			uint32 low = (1<<min);
			uint32 setMask = high-1;
			uint32 clearMask = low-1;

			uint32 result = setMask;
			if (min)
			{
				result &= ~clearMask;
			}
			result |= high;
			result |= low;

			return result;
		}

	};
}

#endif