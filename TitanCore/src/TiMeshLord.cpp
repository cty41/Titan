//------------------------------------------------------------------------------//
// load mesh file or export mesh into binary or ascii file
//------------------------------------------------------------------------------//
#include "TitanStableHeader.h"
#include "TiMeshLord.h"

namespace Titan
{
	MeshLord::MeshLord()
		:mMemPtr(NULL), mFilePtr(NULL)
	{

	}
	//------------------------------------------------------------------------------//
	MeshLord::~MeshLord()
	{
		if(mMemPtr)
		{
			TITAN_DELETE mMemPtr;
			mMemPtr = NULL;
		}

		if(mFilePtr)
		{
			TITAN_DELETE mFilePtr;
			mFilePtr = NULL;
		}
	}
	//------------------------------------------------------------------------------//
	void MeshLord::importFile(const String& filename)
	{
		fopen_s(&mFilePtr, filename.c_str(), "r");
	}
	//------------------------------------------------------------------------------//
	void MeshLord::exportFile(const String& filename)
	{
		fopen_s(&mFilePtr, filename.c_str(), "wb");

		if(mFilePtr == NULL)
		{
			TITAN_EXCEPT(Exception::EXCEP_CANNOT_WRITE_TO_FILE,
				"open mesh file failed : " + filename,
				"MeshLord::exportFile");
			return ;
		}

		//to do
		exportMesh();


	}
	//------------------------------------------------------------------------------//
	void MeshLord::importMesh(/**/)
	{

	}
	//------------------------------------------------------------------------------//
	void MeshLord::exportMesh()
	{
		writeFileHeader();

		writeSubMesh();
		
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeFileHeader()
	{
		//writeInts(MC_HEADER, 1);

		//write mesh size?
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeData(const void* pData, size_t sizeInBytes, size_t num)
	{
		fwrite(pData, sizeInBytes, num, mFilePtr);
	}
	void MeshLord::writeShorts(const short* shorts, size_t num)
	{
		writeData(shorts, sizeof(short), num);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeInts(const int* ints, size_t num)
	{
		writeData(ints, sizeof(int), num);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeFloats(const float* floats, size_t num)
	{
		writeData(floats, sizeof(float), num);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeBools(const bool* bools, size_t num)
	{
		writeData(bools, sizeof(bool), num);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeDoubles(const double* doubles, size_t num)
	{
		writeData(doubles, sizeof(double), num);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeString(const String& str)
	{
		//there has no \n
		fputs(str.c_str(), mFilePtr);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeChunkHeader(uint16 id, uint32 chunkSize)
	{
		writeShorts((short*)&id, 1);

		writeInts((int*)&chunkSize, 1);
	}
	//------------------------------------------------------------------------------//
	void MeshLord::writeSubMesh()
	{

	}
}