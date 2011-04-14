#include "TitanStableHeader.h"
#include "SceneObject.h"
#include "SceneNode.h"

namespace Titan
{
	AutoNamer SceneObject::mAutoNamer("SceneObject_");
	
	SceneObject::SceneObject()
		: mAttachedNode(0)
	{
		mName = mAutoNamer.getAutoName();
	}
	//-------------------------------------------------------------------------------//
	SceneObject::SceneObject(const String& name)
		: mAttachedNode(0), mName(name)
	{

	}
	//-------------------------------------------------------------------------------//
	SceneObject::~SceneObject()
	{

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
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"this object has attached a scene node!!",
				"SceneObject::_setAttachedNode");
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

}