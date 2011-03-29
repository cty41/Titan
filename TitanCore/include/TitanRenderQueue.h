#ifndef __TITAN_RENDERQUEUE_HH
#define __TITAN_RENDERQUEUE_HH

#include "TitanPrerequisites.h"
#include "TitanIteratorWrapper.h"

namespace Titan
{
	class _DllExport RenderQueue : public GeneralAlloc
	{
	public:
		enum RenderGroupType
		{
			RGT_Background	= 0,
			RGT_SceneObject	= 16,
			RGT_HUD			= 128,
		};

		enum SortStrategy
		{
			SS_No_Sort	= 0,
			// priority  shader   texture  distance
			// 8bits	 11bits   13bits   32bits
			SS_Shader_Tex_Dist = 1,

	
		};
	public:
		typedef map<uint, RenderQueueGroup*>::type RenderQueueGroupMap;
		typedef MapIterator<RenderQueueGroupMap> RenderQueueGroupMapIterator;

	public:
		RenderQueue();

		~RenderQueue();

		void	addRenderable(Renderable* rend, uint type, ushort priority);

		//add to default sceneobject group
		void	addRenderable(Renderable* rend);

		void	sort();

		void	clear();

		RenderQueueGroup*	getRenderQueueGroup(uint type);

		void	addRenderQueueGroup(uint type);

		//note: this only affect the RGT_SceneObject, because other groups are only need one simple order to sort
		void	setSortStrategy(SortStrategy ss);

		void	setCurrentCam(Camera *cam){ mCurrentCam = cam; }
		
		Camera*	getCurrentCam() const { return mCurrentCam; }

		RenderQueueGroupMapIterator	getGroupIterator(){ return RenderQueueGroupMapIterator(mRenderQueueGroupMap.begin(), mRenderQueueGroupMap.end());}

	protected:



	protected:
		Camera*					mCurrentCam;	//use to calc the distance
		RenderQueueGroupMap		mRenderQueueGroupMap;
		RenderQueueGroup*		mDefaultGroup;

	};
}
#endif