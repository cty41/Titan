#include "TitanStableHeader.h"
#include "VertexBuffer.h"
#include "Root.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	VertexBuffer::VertexBuffer(HardwareBufferManager* mgr, size_t vertexSize,  
		size_t numVertices, HardwareBuffer::Usage usage, 
		bool useSystemMemory) 
		: HardwareBuffer(usage, useSystemMemory),
		mMgr(mgr),
		mNumVertices(numVertices),
		mVertexSize(vertexSize)
	{
		// Calculate the size of the vertices
		mSizeInBytes = mVertexSize * numVertices;
	}
	//-------------------------------------------------------------//
	VertexBuffer::~VertexBuffer()
	{
		mMgr->_notifyVertexBufferDestroyed(this);
	}
	//-------------------------------------------------------------//
	//
	//class VertexElement
	//
	//-------------------------------------------------------------//
	VertexElement::VertexElement(unsigned short source, size_t offset, 
		VertexElementType theType, VertexElementSemantic semantic, unsigned short index)
		: mSource(source), mOffset(offset), mType(theType), 
		mSemantic(semantic), mIndex(index)
	{
	}
	//-------------------------------------------------------------//
	size_t VertexElement::getSize(void) const
	{
		return getTypeSize(mType);
	}
	//-------------------------------------------------------------//
	size_t VertexElement::getTypeSize(VertexElementType etype)
	{
		switch(etype)
		{
		case VET_COLOR:
		case VET_COLOR_ABGR:
		case VET_COLOR_ARGB:
			return sizeof(RGBA);
		case VET_FLOAT1:
			return sizeof(float);
		case VET_FLOAT2:
			return sizeof(float)*2;
		case VET_FLOAT3:
			return sizeof(float)*3;
		case VET_FLOAT4:
			return sizeof(float)*4;
		case VET_SHORT1:
			return sizeof(short);
		case VET_SHORT2:
			return sizeof(short)*2;
		case VET_SHORT3:
			return sizeof(short)*3;
		case VET_SHORT4:
			return sizeof(short)*4;
		case VET_UBYTE4:
			return sizeof(unsigned char)*4;
		}
		return 0;
	}
	//-------------------------------------------------------------//
	VertexElementType VertexElement::getColorVertexElementType()
	{
		return Root::getSingleton().getActiveRenderer()->getColorVertexElementType();
	}
	//-------------------------------------------------------------//
	//
	//class VertexDeclaration
	//
	//-------------------------------------------------------------//
	const VertexElement* VertexDeclaration::getElement(unsigned short index)
	{
		assert(index < mElementList.size() && "Index out of bounds");

		VertexElementList::iterator i = mElementList.begin();
		for(unsigned short n = 0;n < index; ++n)
			++i;

		return &(*i);
	}
	//-------------------------------------------------------------//
	const VertexElement& VertexDeclaration::addElement(unsigned short source, 
		size_t offset, VertexElementType theType, VertexElementSemantic semantic, 
		unsigned short index /* = 0 */)
	{
		if(theType == VET_COLOR)
		{
			theType = VertexElement::getColorVertexElementType();
		}
		mElementList.push_back(VertexElement(source,offset, theType, semantic, index));
		return mElementList.back();
	}
	//-------------------------------------------------------------//
	const VertexElement& VertexDeclaration::insertElement(unsigned short atPosition, 
		unsigned short source, size_t offset, VertexElementType theType, 
		VertexElementSemantic semantic, unsigned short index /* = 0 */)
	{
		if (atPosition >= mElementList.size())
		{
			return addElement(source, offset, theType, semantic, index);
		}

		VertexElementList::iterator i = mElementList.begin();
		for (unsigned short n = 0; n < atPosition; ++n)
			++i;

		i = mElementList.insert(i, 
			VertexElement(source, offset, theType, semantic, index));
		return *i;
	}
	//-------------------------------------------------------------//
	void VertexDeclaration::removeElement(
		VertexElementSemantic semantic, unsigned short index /* = 0 */)
	{
		assert(index < mElementList.size() && "Index out of bounds");

		VertexElementList::iterator i, iend = mElementList.end();
		for(i = mElementList.begin(); i != iend; ++i)
		{
			if(i->getSemantic() == semantic && i->getIndex() == index)
			{
				mElementList.erase(i);
				break;
			}
		}
	}
	//-------------------------------------------------------------//
	void VertexDeclaration::removeElement(unsigned short elem_index)
	{
		assert(elem_index < mElementList.size() && "Index out of bounds");
		VertexElementList::iterator i = mElementList.begin();
		for (unsigned short n = 0; n < elem_index; ++n)
			++i;
		mElementList.erase(i);
	}
	//-------------------------------------------------------------//
	void VertexDeclaration::removeAllElements()
	{
		mElementList.clear();
	}
	//-------------------------------------------------------------//
	size_t VertexDeclaration::getVertexSize(unsigned short source)
	{
		VertexElementList::const_iterator i, iend;
		iend = mElementList.end();
		size_t sz = 0;

		for (i = mElementList.begin(); i != iend; ++i)
		{
			if (i->getSource() == source)
			{
				sz += i->getSize();

			}
		}
		return sz;
	}
	//-------------------------------------------------------------//
	VertexBufferBinding::VertexBufferBinding(): mHighestIndex(0)
	{
	}
	//-------------------------------------------------------------//
	VertexBufferBinding::~VertexBufferBinding()
	{
		unsetAllBindings();
	}
	//-------------------------------------------------------------//
	void VertexBufferBinding::setBinding(ushort index, const VertexBufferSharedPtr& buffer)
	{
		mBindingMap[index] = buffer;
		mHighestIndex = std::max(mHighestIndex, (ushort)(index));
	}
	//-------------------------------------------------------------//
	void VertexBufferBinding::unsetBinding(ushort index)
	{
		VertexBufferBindingMap::iterator i = mBindingMap.find(index);
		if( i == mBindingMap.end())
		{
			String errorMsg = "Cannot find buffer binding for index " + index;
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				errorMsg,
				"VertexBufferBinding::unsetBinding");
		}
		mBindingMap.erase(i);
	}
	//-------------------------------------------------------------//
	void VertexBufferBinding::unsetAllBindings()
	{
#if 1
		VertexBufferBindingMap::iterator it = mBindingMap.begin(),
			itend = mBindingMap.end();
		while (it != itend)
		{
			it->second.setNull();
			++it;
		}
		mBindingMap.clear();
#endif
		mHighestIndex = 0;
	}
	//-------------------------------------------------------------//
	const VertexBufferBinding::VertexBufferBindingMap& 
		VertexBufferBinding::getBindings() const
	{
		return mBindingMap;
	}
	//-------------------------------------------------------------//
	const VertexBufferSharedPtr& VertexBufferBinding::getBuffer(ushort index)
	{
		VertexBufferBindingMap::iterator i = mBindingMap.find(index);
		if(i != mBindingMap.end())
			return i->second;
		else
		{
			String errorMsg = "can not find specfied vertex buffer bind with index" + index;
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND, errorMsg, "VertexBufferBinding::getBuffer");
		}
	}
	//-------------------------------------------------------------//
	bool VertexBufferBinding::isBufferBound(unsigned short index) const
	{
		return mBindingMap.find(index) != mBindingMap.end();
	}
	//-------------------------------------------------------------//
	ushort VertexBufferBinding::getLastBoundIndex(void) const
	{
		return mBindingMap.empty() ? 0 : mBindingMap.rbegin()->first + 1;
	}

	VertexBufferSharedPtr::VertexBufferSharedPtr(VertexBuffer* buf)
		: SharedPtr<VertexBuffer>(buf)
	{
	}

}