/**
* For the brave souls who get this far: You are the chosen ones,
* the valiant knights of programming who toil away, without rest,
* fixing our most awful code. To you, true saviors, kings of men,
* I say this: never gonna give you up, never gonna let you down,
* never gonna run around and desert you. Never gonna make you cry,
* never gonna say goodbye. Never gonna tell a lie and hurt you.
*/
//just for fun:http://stackoverflow.com/questions/184618/what-is-the-best-comment-in-source-code-you-have-ever-encountered
#ifndef __TITAN_OVERLAY_MGR__HH
#define __TITAN_OVERLAY_MGR__HH

#include "TitanPrerequisites.h"
#include "Singleton.h"


namespace Titan
{
	class _DllExport OverlayMgr : public Singleton<OverlayMgr>, public GeneralAlloc
	{
	public:
		OverlayMgr();

		~OverlayMgr();

		OverlayElement*	createElement(const String& type, const String& name);

		void			removeElement(const String& name);

		void			removeElement(OverlayElement* element);

		void			removeAllElements();

		void			addFactory(OverlayElementFactory* factory);

		void			removeFactory(const String& type);

		void			removeAllFactories();


		void			updateRenderQueue(RenderQueue* queue);

	public:
		typedef map<String, OverlayElement*>::type	OverlayElementMap;
		typedef map<String, OverlayElementFactory*>::type OverlayElementFactoryMap;

	protected:
		OverlayElementMap		mOverlayElementMap;
		OverlayElementFactoryMap	mOverlayElementFactoryMap;
	public:
		OverlayMgr*			getSingletonPtr();

		OverlayMgr&			getSingleton();
	};
}
#endif
