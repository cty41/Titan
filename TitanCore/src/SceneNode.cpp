#include "TitanStableHeader.h"
#include "SceneNode.h"
#include "SceneObject.h"

namespace Titan
{
	AutoNamer SceneNode::msAutoNamer("scenenode_");

	SceneNode::SceneNode(const String& name)
		:mParent(NULL), mName(name),mNeedUpdate(true),
		mScale(Vector3::UNIT_SCALE), mNeedUpdateMat(true),
		mPosition(Vector3::ZERO), mQuaternion(Quaternion::IDENTITY)
	{

	}	
	//-------------------------------------------------------------//
	SceneNode::~SceneNode()
	{
		SceneObjectMap::iterator it = mSceneObjects.begin(),
			iend = mSceneObjects.end();
		while(it != iend)
		{
			TITAN_DELETE it->second;
			++it;
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::attachObject(SceneObject* object)
	{
		SceneObjectMap::iterator it = mSceneObjects.find(object->getName());
		if(it != mSceneObjects.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"this object has attached in this scene node!!",
				"SceneNode::attachObject");
		}
		else
		{
			mSceneObjects.insert(SceneObjectMap::value_type(object->getName(), object));
			object->_setAttachedNode(this);
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::detachObject(SceneObject* object)
	{
		SceneObjectMap::iterator it = mSceneObjects.find(object->getName());
		if(it == mSceneObjects.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"this object called" + object->getName() + "has not attached in this scene node!!",
				"SceneNode::detachObject");
		}
		else
		{
			it->second->detach();
			mSceneObjects.erase(it);
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::detachObject(const String& objectName)
	{
		SceneObjectMap::iterator it = mSceneObjects.find(objectName);
		if(it == mSceneObjects.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, 
				"this object called" + objectName + "has not attached in this scene node!!",
				"SceneNode::detachObject");
		}
		else
		{
			it->second->detach();
			mSceneObjects.erase(it);
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::_setParent(SceneNode* parent)
	{
		if(mParent)
		{
			mParent->removeChild(this);
			mParent = 0;
		}
		
		mParent = parent;
	}
	//-------------------------------------------------------------//
	SceneNode*	SceneNode::createChild(const String& name, const Vector3& v,
		const Quaternion& q)
	{
		SceneNode* node = TITAN_NEW SceneNode(name);
		node->translate(v);
		node->rotate(q);
		addChild(node);

		return node;
	}
	//-------------------------------------------------------------//
	SceneNode* SceneNode::createChild(const Vector3& v,
		const Quaternion& q)
	{
		SceneNode* node = TITAN_NEW SceneNode(msAutoNamer.getAutoName());
		node->translate(v);
		node->rotate(q);
		addChild(node);

		return node;
	}
	//-------------------------------------------------------------//
	void SceneNode::addChild(SceneNode* child)
	{
		if(child->getParent() == this)
			return ;

		mChildrenMap.insert(SceneNodeMap::value_type(child->getName(), child));
		child->_setParent(this);
		
	}
	//-------------------------------------------------------------//
	void SceneNode::removeChild(SceneNode* child)
	{
		if(child)
		{
			SceneNodeMap::iterator it = mChildrenMap.find(child->getName());

			if(it != mChildrenMap.end())
			{
				child->_setParent(0);
				mChildrenMap.erase(it);
			}

		}
	}
	//-------------------------------------------------------------//
	void SceneNode::removeChild(const String& name)
	{
		SceneNodeMap::iterator it = mChildrenMap.find(name);

		if(it != mChildrenMap.end())
		{
			it->second->_setParent(0);
			mChildrenMap.erase(it);
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::notifyUpdate()
	{
		mNeedUpdate = true;
		mNeedUpdateMat = true;
	}
	//-------------------------------------------------------------//
	void SceneNode::_update()
	{
		if(mNeedUpdate)
			_updateFromParent();

		SceneNodeMap::iterator it = mChildrenMap.begin(), itend = mChildrenMap.end();
		for(;it != itend ; ++it)
		{
			it->second->_update();
		}

		mNeedUpdate = false;

		_updateAABB();
	}
	//-------------------------------------------------------------//
	void SceneNode::_updateFromParent()
	{
		if(mParent)
		{
			mDerivedQuaternion = mParent->_getDerivedOrientation() * mQuaternion;

			mDerivedScale = mParent->_getDerivedScale() * mScale;

			mDerivedPosition = mParent->_getDerivedPosition() + mPosition;
		}
		else
		{
			//root node
			mDerivedPosition = mPosition;
			mDerivedQuaternion = mQuaternion;
			mDerivedScale = mScale;
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::_updateAABB()
	{
		mAABB.setNull();
		SceneObjectMap::iterator it = mSceneObjects.begin(), itend = mSceneObjects.end();
		while(it != itend)
		{
			mAABB.merge(it->second->getAABB());
			++it;
		}

		SceneNodeMap::iterator it2 = mChildrenMap.begin(), itend2 = mChildrenMap.end();
		while(it2 != itend2)
		{
			mAABB.merge(it2->second->getAABB());
			++it2;
		}
	}
	//-------------------------------------------------------------//
	void SceneNode::_findVisibleObjects(Camera* cam, SceneMgr::RenderableList& renderableList)
	{
		if(!cam->isVisible(mAABB))
			return;

		SceneObjectMap::iterator it = mSceneObjects.begin(), itend = mSceneObjects.end();
		while(it != itend)
		{
			it->second->_updateRenderableList(renderableList);
			++it;
		}

		SceneNodeMap::iterator it2 = mChildrenMap.begin(), itend2 = mChildrenMap.end();
		while(it2 != itend2)
		{
			it2->second->_findVisibleObjects(cam, renderableList);
			++it2;
		}

	}
	//-------------------------------------------------------------//
	const Matrix4& SceneNode::_getTransformMatrix()
	{
		if(mNeedUpdateMat)
		{
			mTransformMat.makeTransform(
				mDerivedPosition, mDerivedScale, mDerivedQuaternion);
			mNeedUpdateMat = false;
		}

		return mTransformMat;
	}
	//-------------------------------------------------------------//
	void SceneNode::scale(float x, float y, float z)
	{
		scale(Vector3(x, y, z));
	}
	//-------------------------------------------------------------//
	void SceneNode::scale(const Vector3& vec)
	{
		mScale *= vec;

		notifyUpdate();
	}
	//-------------------------------------------------------------//
	void SceneNode::rotate(const Quaternion& q)
	{
		Quaternion qu = q;
		qu.normalise();
		mQuaternion = mQuaternion * qu;
		notifyUpdate();
	}
	//-------------------------------------------------------------//
	void SceneNode::rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.FromAngleAxis(angle,axis);
		rotate(q);
	}
	//-------------------------------------------------------------//
	void SceneNode::translate(float x, float y, float z)
	{
		translate(Vector3(x, y, z));
	}
	//-------------------------------------------------------------//
	void SceneNode::translate(const Vector3& pos)
	{
		mPosition += pos;
		notifyUpdate();
	}
	//-------------------------------------------------------------//
	SceneNode::ObjectIterator SceneNode::getAttachedObjectIterator()
	{
		return ObjectIterator(mSceneObjects.begin(), mSceneObjects.end());
	}
	//-------------------------------------------------------------//
	SceneNode::ConstObjectIterator SceneNode::getAttachedConstObjectIterator()
	{
		return ConstObjectIterator(mSceneObjects.begin(), mSceneObjects.end());
	}
	//-------------------------------------------------------------//
	SceneNode::ChildIterator SceneNode::getChildIterator()
	{
		return ChildIterator(mChildrenMap.begin(), mChildrenMap.end());
	}
	//-------------------------------------------------------------//
	SceneNode::ConstChildIterator SceneNode::getConstChildIterator()
	{
		return ConstChildIterator(mChildrenMap.begin(), mChildrenMap.end());
	}

}