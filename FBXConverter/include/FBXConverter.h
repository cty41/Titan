#ifndef __FBX_CONVERTER__HH
#define __FBX_CONVERTER__HH

#include <vector>
#include <string>

#include <fbxsdk.h>
#include "TiVector3.h"
#include "TitanHelpers.h"



class FBXConverter
{
public:
	FBXConverter(const AcceptArgs& args);

	~FBXConverter();

	void InitializeSdkObjects();

	void DestroySdkObjects();

	bool LoadScene(const std::string& infile);

	void ConvertContent();

	void ConvertNode(KFbxNode* pNode);

	void ConvertMeshData(KFbxNode * pNode);

	void ConvertSkeletonData(KFbxNode* pNode);

	void ConvertSkeleton(KFbxNode * pNode);

	bool ExportScene(const std::string& outfile);

	void ExportMesh(TiSubMesh* mesh);

	KFbxXMatrix ComputeTotalMatrix(KFbxNode* node);

	void CleanSceneData();

protected:
	void WriteDatas(const void* pData, size_t size, size_t count);

	void WriteFloats(const float* pData, size_t count);

	void WriteInts(const int* pData, size_t count);

	void WriteBools(const bool* pData, size_t count);

	void WriteShorts(const short* pData, size_t count);

	void WriteChars(const char* pData, size_t count);

	void WriteChunkHeaders(short id, size_t size);

protected:
	KFbxSdkManager* mpSdkMgr;
	KFbxScene*		mpScene;
	bool			mConvertAll, mConvertAscii;
	std::string		mInFileName, mOutFileName;
	KFbxGeometryConverter*	mFbxGeometryConverter;

	FILE			*mpBinaryFile, *mpAsciiFile;
	TiScene			mTiScene;
};


#endif