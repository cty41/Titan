#ifndef _TITAN_SCENENODE_HH
#define _TITAN_SCENENODE_HH

#include "TiPrerequisites.h"
#include "TiCommon.h"
#include "TiMatrix4.h"
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

		void		setPosition(const Vector3& pos);

		void		setPosition(float x, float y, float z);

		const Vector3&	getPosition() const { return mPosition; }

		const Vector3&	_getDerivedScale();

		const Quaternion& _getDerivedOrientation();

		const Vector3&	  _getDerivedPosition();

		const Matrix4&	 _getTransformMatrix();

		inline void	notifyUpdate();

		inline void notifyUpdateAABB(){ mNeedUpdateAABB = true;}

		void  addToChildrenUpdateList(SceneNode* child);

		void		_update();

		void		_updateFromParent();

		void		_setParent(SceneNode* parent);

		virtual	void _updateWorldBound();

		virtual void _findVisibleObjects(Camera* cam, RenderQueue* queue);

		SceneNode*	getParent() const { return mParent; }

		const String& getName() const { return mName; }

		const AABB& getWorldBound();

		float		getSquaredDistance(Camera* cam);

	protected:
		SceneNode*		mParent;
		String			mName;

		SceneNodeMap	mChildrenMap;
		SceneNodeMap	mChildrenNeedToUpdate;
		SceneObjectMap	mSceneObjects;
		AABB			mWorldBound;
		

		static	AutoNamer	msAutoNamer;

		Quaternion		mQuaternion;
		Vector3			mScale;
		Vector3			mPosition;

		Quaternion		mDerivedQuaternion;
		Vector3			mDerivedScale;
		Vector3			mDerivedPosition;
		//final matrix is used for render coordinate translation
		Matrix4			mTransformMat;
		//The flag indicates whether this node need to update world info(pos, scale, rot. etc..) this frame
		bool			mNeedUpdate;
		//The flag indicates whether this node need to calc its final matrix val this frame
		bool			mNeedUpdateMat;
		//The flag indicates whether this node need to calc its pos, scale, rot from his parent node this frame
		bool			mNeedUpdateParent;
		//The flag indicates whether this node need to update aabb this frame
		bool			mNeedUpdateAABB;


	};
}
#endif