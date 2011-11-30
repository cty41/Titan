#include "DisplayCommon.h"
#include "DisplayMesh.h"

#include <stdio.h>


#define MAT_HEADER_LENGTH 200

void DisplayControlsPoints(KFbxMesh* pMesh);
void DisplayPolygons(KFbxMesh* pMesh);
void DisplayMaterialMapping(KFbxMesh* pMesh);
void DisplayTextureMapping(KFbxMesh* pMesh);
void DisplayTextureNames( KFbxProperty &pProperty, KString& pConnectionString );
void DisplayMaterialConnections(KFbxMesh* pMesh);
void DisplayMaterialTextureConnections( KFbxSurfaceMaterial* pMaterial, 
	char * header, int pMatId, int l );

void DisplayMesh(KFbxNode* pNode)
{
	KFbxMesh* lMesh = (KFbxMesh*) pNode->GetNodeAttribute ();

	DisplayString("Mesh Name: ", (char *) pNode->GetName());
	DisplayControlsPoints(lMesh);
	DisplayPolygons(lMesh);
	DisplayMaterialMapping(lMesh);
	//DisplayMaterial(lMesh);
	//DisplayTexture(lMesh);
	DisplayMaterialConnections(lMesh);
	//DisplayLink(lMesh);
	//DisplayShape(lMesh);
}
