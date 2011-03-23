#ifndef __TITAN_RENDERABLE_HH
#define __TITAN_RENDERABLE_HH

#include "TitanPrerequisites.h"
#include "TitanShaderEffect.h"

namespace Titan
{
	class _DllExport Renderable : public GeneralAlloc
	{
	public:
		class _DllExport RenderableListener
		{
		public:
			RenderableListener(){};

			~RenderableListener(){};

			virtual void customUpdate() = 0;
		};

		typedef list<RenderableListener*>::type RenderableListenerList;
	public:
		Renderable(){};

		virtual ~Renderable(){ mRenderableListenerList.clear(); };

		virtual void			getRenderData(RenderData& rd) = 0;

		virtual void			getTransformMat(Matrix4* transMat) = 0;

		virtual RenderData*		getRenderData() = 0;

		virtual ShaderEffectPtr	getShaderEffect() = 0;

		virtual bool			hasShader() = 0;

		void	addListener(RenderableListener* listener){ mRenderableListenerList.push_back(listener); }
		
		virtual void customUpdate(){};
		void	updateListeners()
		{
			RenderableListenerList::iterator it = mRenderableListenerList.begin(), itend = mRenderableListenerList.end();
			while (it != itend)
			{			
				(*it)->customUpdate();
				++it;
			}
		}
	protected:

		RenderableListenerList	mRenderableListenerList;
	};
}
#endif