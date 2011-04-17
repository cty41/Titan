#ifndef __TITAN_HARDWAREBUFFER_HH
#define __TITAN_HARDWAREBUFFER_HH

#include "TiPrerequisites.h"

namespace Titan
{
	class _DllExport HardwareBuffer : public GeneralAlloc
	{
	public:
		enum Usage 
		    {
                HBU_STATIC = 1,
			    HBU_DYNAMIC = 2,
			    HBU_WRITE_ONLY = 4,
                HBU_DISCARDABLE = 8,
				HBU_STATIC_WRITE_ONLY = 5, 
				HBU_DYNAMIC_WRITE_ONLY = 6,
                HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE = 14


		    };
		    /// Locking options
		    enum LockOptions
		    {
                HBL_NORMAL,
			    HBL_DISCARD,
			    HBL_READ_ONLY,
                HBL_NO_OVERWRITE
		    };
	public:
	  HardwareBuffer(Usage usage, bool systemMemory) 
				: mUsage(usage), mIsLocked(false), mSystemMemory(systemMemory)
            {
            }
            virtual ~HardwareBuffer() {}

		    virtual void* lock(size_t offset, size_t length, LockOptions options)
            {
                mLockOffset = offset;
				mLockSize = length;
				mLockOption = options;
				assert(!isLocked() && "Cannot lock this buffer, it is already locked!");
                void* ret;
				
				// Lock the real buffer if there is no shadow buffer 
				ret = lockImpl(offset, length, options);
                mIsLocked = true;
                
                return ret;
            }

            void* lock(LockOptions options)
            {
                return this->lock(0, mSizeInBytes, options);
            }

		    virtual void unlock(void)
            {
                assert(isLocked() && "Cannot unlock this buffer, it is not locked!");

				// Otherwise, unlock the real one
                unlockImpl();
                mIsLocked = false;
            }

			virtual void readData(size_t offset, size_t length, void* pDest) = 0;

            virtual void writeData(size_t offset, size_t length, const void* pSource,
					bool discardWholeBuffer = false) = 0;

            /// Returns the size of this buffer in bytes
            size_t getSizeInBytes(void) const { return mSizeInBytes; }
            /// Returns the Usage flags with which this buffer was created
            Usage getUsage(void) const { return mUsage; }
			/// Returns whether this buffer is held in system memory
			bool isSystemMemory(void) const { return mSystemMemory; }
            /// Returns whether or not this buffer is currently locked.
            bool isLocked(void) const { return mIsLocked ; }


	protected:
		size_t mSizeInBytes;
		Usage mUsage;
		bool mIsLocked;
		size_t mLockOffset;
		size_t mLockSize;
		bool mSystemMemory;
		LockOptions	mLockOption;


		/// Internal implementation of lock()
		virtual void* lockImpl(size_t offset, size_t length, LockOptions options) = 0;
		/// Internal implementation of unlock()
		virtual void unlockImpl(void) = 0;
	};
}
#endif