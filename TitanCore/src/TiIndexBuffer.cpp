#include "TitanStableHeader.h"
#include "TiIndexBuffer.h"
#include "TiHardwareBufferMgr.h"

namespace Titan
{
	IndexBuffer::IndexBuffer(HardwareBufferMgr* mgr, size_t numIndexes, HardwareBuffer::Usage usage, bool useSystemMemory)
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
	void IndexBuffer::createSingleStripGrid(uint16 xVerts, uint16 yVerts, uint16 xStep, uint16 yStep, uint16 stride)
	{
		int total_strips = yVerts-1;
		int total_indexes_per_strip = xVerts<<1;

		// the total number of indices is equal
		// to the number of strips times the
		// indices used per strip plus one
		// degenerate triangle between each strip
		int total_indexes = (total_indexes_per_strip * total_strips) 
			+ (total_strips<<1) - 2;

		ushort* pIndexValues = new ushort[total_indexes];

		ushort* index = pIndexValues;
		ushort start_vert = 0;
		ushort lineStep = yStep*stride;

		for (int j=0;j<total_strips;++j)
		{
			int k=0;
			ushort vert=start_vert;

			// create a strip for this row
			for (k=0;k<xVerts;++k)
			{
				*(index++) = vert;
				*(index++) = vert + lineStep;
				vert += xStep;
			}
			start_vert += lineStep;

			if (j+1<total_strips)
			{
				// add a degenerate to attach to 
				// the next row
				*(index++) = (vert-xStep)+lineStep;
				*(index++) = start_vert;
			}
		}

		// finally, use the indices we created above
		// to fill the index buffer
		writeData(0, total_indexes * mIndexSize,(void*)pIndexValues);

		// destroy our local data and return
		delete [] pIndexValues;
	}
	//------------------------------------------------------------------------------//
	void IndexBuffer::createTriangleListGrid(uint16 xVerts, uint16 yVerts, uint16 xStep, uint16 yStep, uint16 stride)
	{
		int total_lists = yVerts - 1;
		int total_indexes_per_list = (xVerts - 1) * 6;

		int total_indexes = total_lists * total_indexes_per_list;
		ushort* pIndexValues = new ushort[total_indexes];
		
		ushort* index = pIndexValues;
		ushort start_vert = 0;
		ushort lineStep = yStep * stride;

		for(int j = 0; j < total_lists; ++j)
		{
			int k = 0;
			ushort vert = start_vert;

			for(k = 0; k < xVerts - 1; ++k)
			{
				*(index++) = vert;
				*(index++) = vert + lineStep;
				*(index++) = vert + 1;
				*(index++) = vert + 1;
				*(index++) = vert + lineStep;
				*(index++) = vert + 1 + lineStep;
				++vert;
			}
			start_vert += lineStep;
		}

		writeData(0, total_indexes * mIndexSize,(void*)pIndexValues);

		// destroy our local data and return
		delete [] pIndexValues;
	}

	//-------------------------------------------------------------------------------//
	IndexBufferPtr::IndexBufferPtr(IndexBuffer* buf)
		: SharedPtr<IndexBuffer>(buf)
	{
	}
}