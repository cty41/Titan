#include "TitanStableHeader.h"
#include "TiSceneObject.h"
#include "TiSceneNode.h"
#include "TiRenderQueue.h"

namespace Titan
{
	AutoNamer SceneObject::mAutoNamer("SceneObject_");
	
	SceneObject::SceneObject(SceneObjectFactory* creator)
		: mAttachedNode(0), mRenderQueueID(RGT_SceneObject),
		mRenderQueuePriority(0), mCreator(creator)
	{
		mName = mAutoNamer.getAutoName();
	}
	//-------------------------------------------------------------------------------//
	SceneObject::SceneObject(SceneObjectFactory* creator, const String& name)
		: mAttachedNode(0), mName(name), mCreator(creator)
	{
	}

	const AABB& SceneObject::getWorldBound()
	{
		mWorldBoundBox = getLocalBound();
		mWorldBoundBox.transformAffine(mAttachedNode->_getTransformMatrix());

		return mWorldBoundBox;

	}
	const String& SceneObject::getType() const
	{
		return mCreator->getType();
	}
	//-------------------------------------------------------------------------------//
	bool SceneObject::isAttached() const
	{
		return mAttachedNode ? true : false;
	}
	//-------------------------------------------------------------------------------//
	void SceneObject::detach()
	{
		mAttachedNode->detachObject(this);
		mAttachedNode = 0;
	}
	//-------------------------------------------------------------------------------//
	void SceneObject::_setAttachedNode(SceneNode* node)
	{
		if(mAttachedNode)
		{
			TITAN_EXCEPT_INVALIDPARAMS( "this object has attached a scene node!!");
		}
		else
		{
			mAttachedNode = node;
		}
	}
	//-------------------------------------------------------------------------------//
	const Matrix4& SceneObject::_getAttachedNodeFullTransform(void) const
	{

		if(mAttachedNode)
		{
			// object attached to a sceneNode
			return mAttachedNode->_getTransformMatrix();
		}
		// fallback
		return Matrix4::IDENTITY;
	}
	//------------------------------------------------------------------------------//
	void SceneObject::setRenderQueueID(uint queueId)
	{
		assert(queueId <= RGT_HUD && "queue id is invalid!");
		mRenderQueueID = queueId;
	}
	//------------------------------------------------------------------------------//
	void SceneObject::setRenderQueuePriority(uint8 priority)
	{
		mRenderQueuePriority = priority;
	}
}