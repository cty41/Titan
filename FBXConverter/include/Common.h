#ifndef __COMMON__HH
#define __COMMON__HH

#include <fbxsdk.h>
#include <stdio.h>

void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene);
void DestroySdkObjects(KFbxSdkManager* pSdkManager);
void CreateAndFillIOSettings(KFbxSdkManager* pSdkManager);

bool SaveScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename);



#endif