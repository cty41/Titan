#ifndef _TITAN_SCENEOBJECT_HH
#define _TITAN_SCENEOBJECT_HH

#include "TiPrerequisites.h"
#include "TitanAABB.h"
#include "TiCommon.h"
#include "TiSceneMgr.h"

namespace Titan
{

	class _DllExport SceneObject : public GeneralAlloc
	{
	public:
		SceneObject(SceneObjectFactory* creator, const String& name);

		SceneObject(SceneObjectFactory* creator);

		virtual ~SceneObject(){};

		virtual void _updateRenderQueue(RenderQueue* queue, Camera* cam) = 0;

		virtual const AABB& getLocalBound() const = 0;

		virtual const AABB& getWorldBound();

		const String& getType() const;

		const String& getName() const { return mName; }

		bool isAttached() const;

		void detach();

		void _setAttachedNode(SceneNode* node);

		virtual const Matrix4& _getAttachedNodeFullTransform(void) const;

		SceneNode* getAttachedNode() const { return mAttachedNode; }

		void setRenderQueueID(uint queueId);

		uint getRenderQueueID() const { return mRenderQueueID; }

		void setRenderQueuePriority(uint8 priority);

		uint8 getRenderQueuePriority() const { return mRenderQueuePriority; }

	protected:
		SceneObjectFactory*	mCreator;
		SceneNode*			mAttachedNode;
		String				mName;
		uint				mRenderQueueID;
		uint8				mRenderQueuePriority;
		AABB				mWorldBoundBox;

		static AutoNamer	mAutoNamer;

	};

	class _DllExport SceneObjectFactory : public GeneralAlloc
	{
	public:
		SceneObjectFactory(){};

		virtual ~SceneObjectFactory(){};

		virtual SceneObject*	createInstance(const String& name) = 0;

		virtual void			destroyInstance(SceneObject* object) = 0;

		virtual const String& getType() const = 0;

	};
}
#endif