#include "TitanStableHeader.h"
#include "TiEntity.h"
#include "TiRenderQueue.h"
#include "TiCamera.h"
#include "TiMesh.h"
#include "TitanAABB.h"

namespace Titan
{
	//todo
	EntityPrimitive::EntityPrimitive(Entity* parent, SubMesh* mesh)
	{

	}

	EntityPrimitive::~EntityPrimitive()
	{

	}

	void EntityPrimitive::getRenderData(RenderData& rd)
	{

	}

	void EntityPrimitive::getTransformMat(Matrix4* transMat)
	{

	}

	RenderData* EntityPrimitive::getRenderData()
	{
		return nullptr;
	}

	const MaterialPtr&	EntityPrimitive::getMaterial() const
	{
		return mParent->getMaterial();
	}

	float EntityPrimitive::getSquaredDistance(Camera* cam)
	{
		return 0.0f;
	}

	bool EntityPrimitive::IsUseIdentityView() const
	{
		return false;
	}

	bool EntityPrimitive::IsUseIdentityProj() const
	{
		return false;
	}


	Entity::Entity(SceneObjectFactory* creator, const String& name)
		: SceneObject(creator, name)
	{

	}

	Entity::~Entity()
	{

	}

	//super class override [begin]
	const AABB& Entity::getLocalBound() const 
	{
		//todo, use 
		AABB aabb;
		return aabb;
	}

	void Entity::_updateRenderQueue(RenderQueue* queue, Camera* cam)
	{

	}
	//super class override [End]

	void Entity::setMaterial(const String& name)
	{
		//todo
	}

	const MaterialPtr&	Entity::getMaterial() const
	{
		return mMaterialPtr;
	}

}