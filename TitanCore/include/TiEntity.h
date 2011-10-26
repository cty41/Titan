#ifndef _TITAN_ENTITY__HH
#define _TITAN_ENTITY__HH

#include "TiPrerequisites.h"
#include "TiSceneObject.h"
#include "TiRenderable.h"

namespace Titan
{
	class _DllExport EntityPrimitive : public Renderable
	{
	public:
		EntityPrimitive(Entity* parent, SubMesh* mesh);

		~EntityPrimitive();

		//super class override [begin]
		virtual void			getRenderData(RenderData& rd);

		virtual void			getTransformMat(Matrix4* transMat);

		virtual RenderData*		getRenderData();

		virtual const MaterialPtr&	getMaterial() const;

		virtual float			getSquaredDistance(Camera* cam);

		virtual bool			IsUseIdentityView() const;

		virtual bool			IsUseIdentityProj() const;
		//super class override [End]


	};

	class _DllExport Entity :public SceneObject
	{
	public:
		Entity(SceneObjectFactory* creator, const String& name);

		~Entity();

		//super class override [begin]
		void	_updateRenderQueue(RenderQueue* queue, Camera* cam);

		const AABB& getLocalBound() const;
		//super class override [End]

		void setMaterial(const String& name);


	protected:
		Mesh*	mMesh;
	};
}
#endif