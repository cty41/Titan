#ifndef __SharedPtr_H__
#define __SharedPtr_H__

#include "TiPrerequisites.h"

namespace Titan 
{
	/// The method to use to free memory on destruction
	enum SharedPtrFreeMethod
	{
		/// Use TITAN_DELETE to free the memory
		SPFM_DELETE,
		/// Use TITAN_DELETE_T to free (only MEMCATEGORY_GENERAL supported)
		SPFM_DELETE_T,
		/// Use TITAN_FREE to free (only MEMCATEGORY_GENERAL supported)
		SPFM_FREE
	};

	template<class T> class SharedPtr
	{
	protected:
		T* pRep;
		unsigned int* pUseCount;
		SharedPtrFreeMethod useFreeMethod; // if we should use TITAN_FREE instead of TITAN_DELETE
	public:
		/** Constructor, does not initialise the SharedPtr.
			@remarks
				<b>Dangerous!</b> You have to call bind() before using the SharedPtr.
		*/
		SharedPtr() : pRep(0), pUseCount(0), useFreeMethod(SPFM_DELETE)
        {
        }

		/** Constructor.
		@param rep The pointer to take ownership of
		@param freeMode The mechanism to use to free the pointer
		*/
        template< class Y>
		explicit SharedPtr(Y* rep, SharedPtrFreeMethod freeMethod = SPFM_DELETE) 
			: pRep(rep)
			, pUseCount(rep ? TITAN_NEW_T(unsigned int, MEMCATEGORY_GENERAL)(1) : 0)
			, useFreeMethod(freeMethod)
		{
			if (rep)
			{
			}
		}
		SharedPtr(const SharedPtr& r)
            : pRep(0), pUseCount(0), useFreeMethod(SPFM_DELETE)
		{
            
			    pRep = r.pRep;
			    pUseCount = r.pUseCount; 
				useFreeMethod = r.useFreeMethod;
			    // Handle zero pointer gracefully to manage STL containers
			    if(pUseCount)
			    {
				    ++(*pUseCount); 
			    }
            
		}
		SharedPtr& operator=(const SharedPtr& r) {
			if (pRep == r.pRep)
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			swap(tmp);
			return *this;
		}
		
		template< class Y>
		SharedPtr(const SharedPtr<Y>& r)
            : pRep(0), pUseCount(0), useFreeMethod(SPFM_DELETE)
		{
			    pRep = r.getPointer();
			    pUseCount = r.useCountPointer();
				useFreeMethod = r.freeMethod();
			    // Handle zero pointer gracefully to manage STL containers
			    if(pUseCount)
			    {
				    ++(*pUseCount);
			    }
		}
		template< class Y>
		SharedPtr& operator=(const SharedPtr<Y>& r) {
			if (pRep == r.getPointer())
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			swap(tmp);
			return *this;
		}
		virtual ~SharedPtr() {
            release();
		}


		inline T& operator*() const { assert(pRep); return *pRep; }
		inline T* operator->() const { assert(pRep); return pRep; }
		inline T* get() const { return pRep; }

		/** Binds rep to the SharedPtr.
			@remarks
				Assumes that the SharedPtr is uninitialised!
		*/
		void bind(T* rep, SharedPtrFreeMethod freeMethod = SPFM_DELETE) {
			assert(!pRep && !pUseCount);
			pUseCount = TITAN_NEW_T(unsigned int, MEMCATEGORY_GENERAL)(1);
			pRep = rep;
			useFreeMethod = freeMethod;
		}

		inline bool unique() const {  assert(pUseCount); return *pUseCount == 1; }
		inline unsigned int useCount() const {  assert(pUseCount); return *pUseCount; }
		inline unsigned int* useCountPointer() const { return pUseCount; }

		inline T* getPointer() const { return pRep; }
		inline SharedPtrFreeMethod freeMethod() const { return useFreeMethod; }

		inline bool isNull(void) const { return pRep == 0; }

        inline void setNull(void) { 
			if (pRep)
			{
				// can't scope lock mutex before release in case deleted
				release();
				pRep = 0;
				pUseCount = 0;
			}
        }

    protected:

        inline void release(void)
        {
			bool destroyThis = false;

            /* If the mutex is not initialized to a non-zero value, then
               neither is pUseCount nor pRep.
             */

 
				if (pUseCount)
				{
					if (--(*pUseCount) == 0) 
					{
						destroyThis = true;
	                }
				}
         
			if (destroyThis)
				destroy();

        }

        virtual void destroy(void)
        {
            // IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS
            // BEFORE SHUTTING Titan DOWN
            // Use setNull() before shutdown or make sure your pointer goes
            // out of scope before Titan shuts down to avoid this.
			switch(useFreeMethod)
			{
			case SPFM_DELETE:
				TITAN_DELETE pRep;
				break;
			case SPFM_DELETE_T:
				TITAN_DELETE_T(pRep, T, MEMCATEGORY_GENERAL);
				break;
			case SPFM_FREE:
				TITAN_FREE(pRep, MEMCATEGORY_GENERAL);
				break;
			};
			// use TITAN_FREE instead of TITAN_DELETE_T since 'unsigned int' isn't a destructor
			// we only used TITAN_NEW_T to be able to use constructor
            TITAN_FREE(pUseCount, MEMCATEGORY_GENERAL);
        }

		virtual void swap(SharedPtr<T> &other) 
		{
			std::swap(pRep, other.pRep);
			std::swap(pUseCount, other.pUseCount);
			std::swap(useFreeMethod, other.useFreeMethod);
		}
	};

	template<class T, class U> inline bool operator==(SharedPtr<T> const& a, SharedPtr<U> const& b)
	{
		return a.get() == b.get();
	}

	template<class T, class U> inline bool operator!=(SharedPtr<T> const& a, SharedPtr<U> const& b)
	{
		return a.get() != b.get();
	}

	template<class T, class U> inline bool operator<(SharedPtr<T> const& a, SharedPtr<U> const& b)
	{
		return std::less<const void*>()(a.get(), b.get());
	}
}



#endif
