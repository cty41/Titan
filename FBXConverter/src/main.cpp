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
	if(argc < 2)
	{
		printHelpInfos();
		return 0;
	}

	AcceptArgs args;
	parseArgs(args, argc, argv);

	FBXConverter* pFbxConverter = new FBXConverter(args);

	delete pFbxConverter;
	return 0;
}


