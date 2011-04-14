#include "TitanStableHeader.h"
#include "IndexBuffer.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	IndexBuffer::IndexBuffer(HardwareBufferManager* mgr, size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory)
		: HardwareBuffer(usage, useSystemMemory), mMgr(mgr), mNumIndexes(numIndexes)
	{
		//change in the future
		mIndexSize = sizeof(unsigned short);
		mSizeInBytes = mIndexSize * mNumIndexes;
	}
	//-------------------------------------------------------------------------------//
	IndexBuffer::~IndexBuffer()
	{
		mMgr->_notifyIndexBufferDestroyed(this);
	}
	//-------------------------------------------------------------------------------//
	void IndexBuffer::createSingleStripGrid(uint16 xVerts, uint16 yVerts, uint16 xStep, uint16 yStep, uint16 stride, uint16 flags)
	{
		int total_strips = 
			yVerts-1;
		int total_indexes_per_strip = 
			xVerts<<1;

		// the total number of indices is equal
		// to the number of strips times the
		// indices used per strip plus one
		// degenerate triangle between each strip
		int total_indexes = 
			(total_indexes_per_strip * total_strips) 
			+ (total_strips<<1) - 2;

		unsigned short* pIndexValues = 
			new unsigned short[total_indexes];

		unsigned short* index = pIndexValues;
		unsigned short start_vert = 0;
		unsigned short lineStep = yStep*stride;

		int _num = 0;
		for (int j=0;j<total_strips;++j)
		{
			int k=0;
			unsigned short vert=start_vert;
			if(_num >= 78)
			{
				_num = _num;
			}
			// create a strip for this row
			for (k=0;k<xVerts;++k)
			{
				*(index++) = vert;
				*(index++) = vert + lineStep;
				_num += 2;
				vert += xStep;
			}
			start_vert += lineStep;

			if (j+1<total_strips)
			{
				// add a degenerate to attach to 
				// the next row
				*(index++) = (vert-xStep)+lineStep;
				*(index++) = start_vert;
				_num += 2;
			}
		}

		// finally, use the indices we created above
		// to fill the index buffer
		writeData(0, total_indexes * mIndexSize,(void*)pIndexValues);

		// destroy our local data and return
		delete [] pIndexValues;
	}


	//-------------------------------------------------------------------------------//
	IndexBufferSharedPtr::IndexBufferSharedPtr(IndexBuffer* buf)
		: SharedPtr<IndexBuffer>(buf)
	{
	}
}