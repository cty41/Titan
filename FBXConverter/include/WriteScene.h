#ifndef  __WRITE_SCENE__HH
#define __WRITE_SCENE__HH

#include <fbxsdk.h>

void WriteScene(KFbxScene* pScene);

void WriteScene(KFbxNode* pNode);

void WriteMesh(KFbxNode* pNode);

#endif