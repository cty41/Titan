#ifndef __TITAN_MATERIAL__HH
#define __TITAN_MATERIAL__HH

#include "TiPrerequisites.h"
#include "TiResource.h"
#include "TiSharedPtr.h"
#include "TiIteratorWrapper.h"
#include "TiPass.h"

namespace Titan
{
	class _DllExport Material : public Resource
	{
	public:
		typedef vector<Pass*>::type		PassVec;
		typedef VectorIterator<PassVec>	PassVecIterator;
	public:
		Material(ResourceMgr* mgr, const String& name, ResourceHandle id, const String& group, bool isManual);

		virtual ~Material();

		bool	isTransparent() const { return mPassVec.at(0)->isTransparent();}

		bool	isTransparentSorted() const { return mPassVec.at(0)->isTransparentSort();}

		void	setDepthWrite(bool enable);

		Pass*	createPass();

		void	removeAllPasses();
		
		PassVecIterator	getPassIterator(){ return PassVecIterator(mPassVec.begin(), mPassVec.end()); }

		Pass*	getPass(size_t index);


	protected:

		virtual void loadImpl();

		virtual void unloadImpl();

	protected:
		//set default material at the creation of material
		void	setDefault();

	protected:
		PassVec		mPassVec;
	};

	class _DllExport MaterialPtr : public SharedPtr<Material>
	{
	public:
		MaterialPtr()
			:SharedPtr<Material>(){}

		explicit MaterialPtr(Material* mtrl)
			:SharedPtr<Material>(mtrl){}

		MaterialPtr(const MaterialPtr& r)
			:SharedPtr<Material>(r){}

		MaterialPtr(const ResourcePtr& r)
			:SharedPtr<Material>()
		{
			pRep = static_cast<Material*>(r.getPointer());
			pUseCount = r.useCountPointer();
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

			pRep = static_cast<Material*>(r.getPointer());
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