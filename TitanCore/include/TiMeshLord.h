//------------------------------------------------------------------------------//
// load mesh file or export mesh into binary or ascii file
//------------------------------------------------------------------------------//
#ifndef _TITAN_MESH_LORD__HH
#define _TITAN_MESH_LORD__HH

#include "TiPrerequisites.h"


namespace Titan
{
	class _DllExport MeshLord : public GeneralAlloc
	{
	public:
		MeshLord();

		~MeshLord();

		void importFile(const String& filename);

		void exportFile(const String& filename);

		void importMesh();

		void exportMesh();

	protected:
		void writeFileHeader();

		void writeChunkHeader(uint16 id, uint32 chunkSize);

		void writeString(const String& str);

		void writeBools(const bool* bools, size_t num);

		void writeShorts(const short* shorts, size_t num);

		void writeInts(const int* ints, size_t num);

		void writeFloats(const float* floats, size_t num);

		void writeDoubles(const double* doubles, size_t num);

		void writeData(const void* pData, size_t sizeInBytes, size_t num);

		void writeSubMesh();

		void writeIndexData();

		void writeVertexDecl();

		void writeVertexData();




	protected:
		MemoryDataStream*	mMemPtr;
		FILE*		mFilePtr;
	};
}
#endif