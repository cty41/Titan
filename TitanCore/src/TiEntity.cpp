#include "TitanStableHeader.h"
#include "TiEntity.h"
#include "TiRenderQueue.h"
#include "TiCamera.h"

namespace Titan
{
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
		return AABB::BOX_NULL;
	}

	void Entity::_updateRenderQueue(RenderQueue* queue, Camera* cam)
	{

	}
	//super class override [End]

	void Entity::setMaterial(const String& name)
	{
		//todo
	}
}