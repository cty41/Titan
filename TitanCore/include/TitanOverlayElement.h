#ifndef __TITAN_OVERLAY_ELEMENT__HH
#define __TITAN_OVERLAY_ELEMENT__HH

#include "TitanPrerequisites.h"
#include "Renderable.h"

namespace Titan
{
	class _DllExport OverlayElement : public Renderable
	{
	public:
		OverlayElement(const String& name);

		virtual ~OverlayElement(){};

		const String& getName() const { return mName; }

		ushort	getZOrder() const { return mZOrder; }

	protected:
		String	mName;
		ushort	mZOrder;
	};


	class _DllExport OverlayElementFactory : public GeneralAlloc
	{
	public:
		OverlayElementFactory(){}

		virtual ~OverlayElementFactory(){}

		const String& getTypeName() const { return mTypeName; }

		virtual OverlayElement* createInstance(const String& name) = 0;

		virtual void		destroyInstance(OverlayElement* element) = 0;
	protected:
		String		mTypeName;
	};
}

#endif