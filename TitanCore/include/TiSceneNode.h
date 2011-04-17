#ifndef _TITAN_SCENENODE_HH
#define _TITAN_SCENENODE_HH

#include "TiPrerequisites.h"
#include "TitanCommon.h"
#include "TitanMatrix4.h"
#include "TiIteratorWrapper.h"
#include "TitanAABB.h"
#include "TiSceneMgr.h"

namespace Titan
{
	class _DllExport SceneNode : public GeneralAlloc
	{
	public:
		typedef hash_map<String, SceneNode*>::type SceneNodeMap;
		typedef hash_map<String, SceneObject*>::type SceneObjectMap;
		typedef MapIterator<SceneObjectMap> ObjectIterator;
		typedef ConstMapIterator<SceneObjectMap> ConstObjectIterator;
		typedef MapIterator<SceneNodeMap> ChildIterator;
		typedef ConstMapIterator<SceneNodeMap> ConstChildIterator;
	public:
		SceneNode(const String& name);
		
		virtual ~SceneNode();

		virtual SceneNode*	createChild(const String& name, const Vector3& v = Vector3::ZERO,
			const Quaternion& q = Quaternion::IDENTITY);

		virtual SceneNode*	createChild(const Vector3& v = Vector3::ZERO,
			const Quaternion& q = Quaternion::IDENTITY);

		virtual void		removeChild(const String& name);

		virtual void		removeChild(SceneNode* child);

		void				attachObject(SceneObject* object);

		void				detachObject(const String& objectName);

		void				detachObject(SceneObject* object);

		void				addChild(SceneNode* child);


		void		scale(float x, float y, float z);

		void		scale(const Vector3& vec);

		const Vector3& getScale() const { return mScale;}

		void		rotate(const Quaternion& q);

		void		rotate(const Vector3& axis, const Radian& angle);

		const Quaternion& getOrientation() const { return mQuaternion; }

		void		translate(float x, float y, float z);

		void		translate(const Vector3& pos);

		const Vector3&	getPosition() const { return mPosition; }

		const Vector3&	_getDerivedScale() const { return mDerivedScale; }

		const Quaternion& _getDerivedOrientation() const { return mDerivedQuaternion; }

		const Vector3&	  _getDerivedPosition() const { return mDerivedPosition; }

		const Matrix4&	 _getTransformMatrix();

		void		notifyUpdate();

		void		_update();

		void		_updateFromParent();

		void		_setParent(SceneNode* parent);

		virtual	void _updateAABB();

		virtual void _findVisibleObjects(Camera* cam, RenderQueue* queue);

		SceneNode*	getParent() const { return mParent; }

		const String& getName() const { return mName; }

		const AABB& getAABB() const { return mAABB; }

		float		getSquaredDistance(Camera* cam);

		virtual ObjectIterator 	getAttachedObjectIterator();

		virtual ConstObjectIterator getAttachedConstObjectIterator();

		virtual ChildIterator 	getChildIterator();

		virtual ConstChildIterator getConstChildIterator();

	protected:
		SceneNode*		mParent;
		String			mName;

		SceneNodeMap	mChildrenMap;
		SceneObjectMap	mSceneObjects;
		AABB			mAABB;
		

		static	AutoNamer	msAutoNamer;

		Quaternion		mQuaternion;
		Vector3			mScale;
		Vector3			mPosition;

		Quaternion		mDerivedQuaternion;
		Vector3			mDerivedScale;
		Vector3			mDerivedPosition;

		Matrix4			mTransformMat;

		bool			mNeedUpdate;
		bool			mNeedUpdateMat;

	};
}
#endif