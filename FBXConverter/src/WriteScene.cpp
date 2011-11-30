#include "WriteScene.h"



void WriteScene(KFbxScene* pScene)
{
	int i;
	KFbxNode* lNode = pScene->GetRootNode();

	if(lNode)
	{
		for(i = 0; i < lNode->GetChildCount(); i++)
		{
			WriteScene(lNode->GetChild(i));
		}
	}
}

void WriteScene(KFbxNode* pNode)
{
	KFbxNodeAttribute::EAttributeType lAttributeType;
	int i;

	if(pNode->GetNodeAttribute() == NULL)
	{
		printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
		switch(lAttributeType)
		{
			//case KFbxNodeAttribute::eMARKER:  
			//	DisplayMarker(pNode);
			//	break;

			//case KFbxNodeAttribute::eSKELETON:  
			//	DisplaySkeleton(pNode);
			//	break;

		case KFbxNodeAttribute::eMESH:      
			WriteMesh(pNode);
			break;

			//case KFbxNodeAttribute::eNURB:      
			//	DisplayNurb(pNode);
			//	break;

			//case KFbxNodeAttribute::ePATCH:     
			//	DisplayPatch(pNode);
			//	break;

			//case KFbxNodeAttribute::eCAMERA:    
			//	DisplayCamera(pNode);
			//	break;

			//case KFbxNodeAttribute::eLIGHT:     
			//	DisplayLight(pNode);
			//	break;

			//case KFbxNodeAttribute::eLODGROUP:
			//	DisplayLodGroup(pNode);
			//	break;
		}
	}

	for(i = 0; i < pNode->GetChildCount(); ++i)
	{
		WriteScene(pNode->GetChild(i));
	}
}

void WriteMesh(KFbxNode* pNode)
{

}