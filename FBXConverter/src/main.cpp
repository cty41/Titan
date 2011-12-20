#include <string>
#include <fbxsdk.h>

#include "TitanHelpers.h"
#include "FBXConverter.h"

void printHelpInfos()
{
	printf("Titan FBX Converter \n");
	printf("-a convert all fbx file to .xmesh \n");
	printf("[in:] source fbx file\n");
	printf("[out:] dest xmesh file\n");
}

int main(int argc, char** argv)
{
	AcceptArgs args;
	if(!parseArgs(args, argc, argv))
	{
		printHelpInfos();
		return 0;
	}

	FBXConverter* pFbxConverter = new FBXConverter(args);

	delete pFbxConverter;
	return 0;
}


