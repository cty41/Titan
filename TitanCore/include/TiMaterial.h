#ifndef __TITAN_MATERIAL__HH
#define __TITAN_MATERIAL__HH

#include "TitanPrerequisites.h"
#include "TitanResource.h"
#include "TitanSharedPtr.h"

namespace Titan
{
	class _DllExport Material : public Resource
	{
	public:
		Material(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group);

		~Material();

	protected:
		virtual void prepareImpl();

		virtual void unprepareImpl();

		virtual void loadImpl();

		virtual void unloadImpl();

	protected:
	};

	class _DllExport MaterialPtr : public SharedPtr<Material>
	{
	public:
		MaterialPtr()
			:SharedPtr<Material>(){}

		explicit MaterialPtr(Material* mtrl, SharedPtrFreeMethod freeMethod = SPFM_DELETE)
			:SharedPtr<Material>(mtrl, freeMethod){}

		MaterialPtr(const MaterialPtr& r)
			:SharedPtr<Material>(r){}

		MaterialPtr(const ResourcePtr& r)
			:SharedPtr<Material>()
		{
			pRep = static_cast<Material*>(r.getPointer());
			pUseCount = ++r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}

		MaterialPtr& operator=(const ResourcePtr& r)
		{
			if(pRep == static_cast<Material*>(r.getPointer()))
				return *this;
			release();

			pRep = static_cast<Texture*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
			return *this;
		}

	};
}
#endif