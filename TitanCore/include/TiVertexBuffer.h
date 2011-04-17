#ifndef _TITAN_VERTEXBUFFER_H
#define _TITAN_VERTEXBUFFER_H

#include "TiPrerequisites.h"
#include "TitanCommon.h"
#include "TiHardwareBuffer.h"
#include "TiSharedPtr.h"
#include "TitanColor.h"

namespace Titan
{
	class _DllExport VertexBuffer : public HardwareBuffer
	{
	public:
		VertexBuffer(HardwareBufferMgr *mgr, size_t vertexSize, size_t numVertices,
			VertexBuffer::Usage usage, bool useSystemMemory);
		virtual ~VertexBuffer();

		size_t getVertexSize(void) const { return mVertexSize; }
		/// Get the number of vertices in this buffer
		size_t getNumVertices(void) const { return mNumVertices; }
	protected:
		HardwareBufferMgr*	mMgr;
		size_t mNumVertices;
		size_t mVertexSize;

	};

	class _DllExport VertexBufferSharedPtr : public SharedPtr<VertexBuffer>
	{
	public:
		VertexBufferSharedPtr() : SharedPtr<VertexBuffer>() {}
		explicit VertexBufferSharedPtr(VertexBuffer* buf);

	};

	class _DllExport VertexElement : public GeneralAlloc
    {
    protected:
        /// The source vertex buffer, as bound to an index using VertexBufferBinding
        unsigned short mSource;
        /// The offset in the buffer that this element starts at
        size_t mOffset;
        /// The type of element
        VertexElementType mType;
        /// The meaning of the element
        VertexElementSemantic mSemantic;
        /// Index of the item, only applicable for some elements like texture coords
        unsigned short mIndex;
    public:
		/// Constructor, should not be called directly, only needed because of list
		VertexElement() {}
        /// Constructor, should not be called directly, call VertexDeclaration::addElement
        VertexElement(unsigned short source, size_t offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);

		unsigned short getSource() const { return mSource; }

		size_t getOffset() const { return mOffset; }

		VertexElementType getType() const { return mType; }

		VertexElementSemantic getSemantic() const { return mSemantic; }

		unsigned short getIndex() const { return mIndex; }

		size_t getSize(void) const;

		static size_t getTypeSize(VertexElementType etype);

		static uint32 convertColor(const Color& src,
			VertexElementType dst);


		static VertexElementType getColorVertexElementType();

		inline bool operator== (const VertexElement& rhs) const
		{
			if (mType != rhs.mType ||
				mIndex != rhs.mIndex ||
				mOffset != rhs.mOffset ||
				mSemantic != rhs.mSemantic ||
				mSource != rhs.mSource)
				return false;
			else
				return true;

		}

		inline void baseVertexPointerToElement(void* pBase, void** pElem) const
        {
            // The only way we can do this is to cast to char* in order to use byte offset
            // then cast back to void*.
            *pElem = static_cast<void*>(
            	static_cast<unsigned char*>(pBase) + mOffset);
        }

        inline void baseVertexPointerToElement(void* pBase, float** pElem) const
        {
            // The only way we can do this is to cast to char* in order to use byte offset
            // then cast back to float*. However we have to go via void* because casting
            // directly is not allowed
            *pElem = static_cast<float*>(
                static_cast<void*>(
                    static_cast<unsigned char*>(pBase) + mOffset));
        }

        inline void baseVertexPointerToElement(void* pBase, RGBA** pElem) const
        {
            *pElem = static_cast<RGBA*>(
                static_cast<void*>(
                    static_cast<unsigned char*>(pBase) + mOffset));
		}

        inline void baseVertexPointerToElement(void* pBase, unsigned char** pElem) const
        {
            *pElem = static_cast<unsigned char*>(pBase) + mOffset;
        }

        
        inline void baseVertexPointerToElement(void* pBase, unsigned short** pElem) const
        {
			*pElem = static_cast<unsigned short*>(
				static_cast<void*>(
					static_cast<unsigned char*>(pBase) + mOffset));
        }
        
    };

	class _DllExport VertexDeclaration : public GeneralAlloc
    {
    public:
		/// Defines the list of vertex elements that makes up this declaration
        typedef list<VertexElement>::type VertexElementList;
 
    protected:
        VertexElementList mElementList;
    public:
        /// Standard constructor, not you should use HardwareBufferMgr::createVertexDeclaration
		VertexDeclaration(){};
		virtual ~VertexDeclaration(){};

        size_t getElementCount(void) { return mElementList.size(); }
        /** Gets read-only access to the list of vertex elements. */
		const VertexElementList& getElements(void) const { return mElementList; }
        /** Get a single element. */
        const VertexElement* getElement(unsigned short index);

        virtual const VertexElement& addElement(unsigned short source, size_t offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);
        
        virtual const VertexElement& insertElement(unsigned short atPosition,
            unsigned short source, size_t offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);

        /** Remove the element at the given index from this declaration. */
        virtual void removeElement(unsigned short elem_index);

        /** Remove the element with the given semantic and usage index.
        @remarks
            In this case 'index' means the usage index for repeating elements such
            as texture coordinates. For other elements this will always be 0 and does
            not refer to the index in the vector.
        */
        virtual void removeElement(VertexElementSemantic semantic, unsigned short index = 0);

		/** Remove all elements. */
		virtual void removeAllElements(void);

		/** Gets the vertex size defined by this declaration for a given source. */
        virtual size_t getVertexSize(unsigned short source);


        inline bool operator== (const VertexDeclaration& rhs) const
        {
            if (mElementList.size() != rhs.mElementList.size())
                return false;

            VertexElementList::const_iterator i, iend, rhsi, rhsiend;
            iend = mElementList.end();
            rhsiend = rhs.mElementList.end();
            rhsi = rhs.mElementList.begin();
            for (i = mElementList.begin(); i != iend && rhsi != rhsiend; ++i, ++rhsi)
            {
                if ( !(*i == *rhsi) )
                    return false;
            }

            return true;
        }
        inline bool operator!= (const VertexDeclaration& rhs) const
        {
            return !(*this == rhs);
        }
    };

	class _DllExport VertexBufferBinding : public GeneralAlloc
	{
	public:
		typedef  map<ushort, VertexBufferSharedPtr>::type VertexBufferBindingMap;
	protected:
		VertexBufferBindingMap mBindingMap;
		ushort	mHighestIndex;
	public:
		VertexBufferBinding();

		virtual ~VertexBufferBinding();

		virtual void setBinding(ushort index, const VertexBufferSharedPtr& buffer);

		virtual void unsetBinding(ushort index);

		virtual void unsetAllBindings();

		virtual const VertexBufferBindingMap& getBindings() const;

		virtual const VertexBufferSharedPtr&  getBuffer(ushort index) ;

		virtual bool  isBufferBound(ushort index) const;

		virtual unsigned short getNextIndex(void) const { return mHighestIndex + 1; }

		virtual unsigned short getLastBoundIndex(void) const;

	};
}

#endif