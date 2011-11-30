#include <fbxsdk.h>

#include <stdio.h>
#include <math.h>
#include "DisplayCommon.h"


void DisplayString(const char* pHeader, const char* pValue /* = "" */, const char* pSuffix /* = "" */)
{
	KString lString;

	lString = pHeader;
	lString += pValue;
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix /* = "" */)
{
	KString lString;

	lString = pHeader;
	lString += pValue ? "true" : "false";
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void DisplayInt(const char* pHeader, int pValue, const char* pSuffix /* = "" */)
{
	KString lString;

	lString = pHeader;
	lString += pValue;
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix /* = "" */)
{
	KString lString;
	KString lFloatValue = (float) pValue;

	lFloatValue = pValue <= -HUGE_VAL ? "-INFINITY" : lFloatValue.Buffer();
	lFloatValue = pValue >=  HUGE_VAL ?  "INFINITY" : lFloatValue.Buffer();

	lString = pHeader;
	lString += lFloatValue;
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void Display2DVector(const char* pHeader, KFbxVector2 pValue, const char* pSuffix  /* = "" */)
{
	KString lString;
	KString lFloatValue1 = (float)pValue[0];
	KString lFloatValue2 = (float)pValue[1];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void Display3DVector(const char* pHeader, KFbxVector4 pValue, const char* pSuffix /* = "" */)
{
	KString lString;
	KString lFloatValue1 = (float)pValue[0];
	KString lFloatValue2 = (float)pValue[1];
	KString lFloatValue3 = (float)pValue[2];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >=  HUGE_VAL ?  "INFINITY" : lFloatValue3.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}

void Display4DVector(const char* pHeader, KFbxVector4 pValue, const char* pSuffix /* = "" */)
{
	KString lString;
	KString lFloatValue1 = (float)pValue[0];
	KString lFloatValue2 = (float)pValue[1];
	KString lFloatValue3 = (float)pValue[2];
	KString lFloatValue4 = (float)pValue[3];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >=  HUGE_VAL ?  "INFINITY" : lFloatValue3.Buffer();
	lFloatValue4 = pValue[3] <= -HUGE_VAL ? "-INFINITY" : lFloatValue4.Buffer();
	lFloatValue4 = pValue[3] >=  HUGE_VAL ?  "INFINITY" : lFloatValue4.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += ", ";
	lString += lFloatValue4;
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void DisplayColor(const char* pHeader, KFbxPropertyDouble3 pValue, const char* pSuffix /* = "" */)

{
	KString lString;

	lString = pHeader;
	//lString += (float) pValue.mRed;
	//lString += (double)pValue.GetArrayItem(0);
	lString += " (red), ";
	//lString += (float) pValue.mGreen;
	//lString += (double)pValue.GetArrayItem(1);
	lString += " (green), ";
	//lString += (float) pValue.mBlue;
	//lString += (double)pValue.GetArrayItem(2);
	lString += " (blue)";
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void DisplayColor(const char* pHeader, KFbxColor pValue, const char* pSuffix /* = "" */)
{
	KString lString;

	lString = pHeader;
	lString += (float) pValue.mRed;

	lString += " (red), ";
	lString += (float) pValue.mGreen;

	lString += " (green), ";
	lString += (float) pValue.mBlue;

	lString += " (blue)";
	lString += pSuffix;
	lString += "\n";
	printf(lString);
}


void DisplayMetaData(KFbxScene* pScene)
{
	KFbxDocumentInfo* sceneInfo = pScene->GetSceneInfo();
	if (sceneInfo)
	{
		printf("\n\n--------------------\nMeta-Data\n--------------------\n\n");
		printf("    Title: %s\n", sceneInfo->mTitle.Buffer());
		printf("    Subject: %s\n", sceneInfo->mSubject.Buffer());
		printf("    Author: %s\n", sceneInfo->mAuthor.Buffer());
		printf("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
		printf("    Revision: %s\n", sceneInfo->mRevision.Buffer());
		printf("    Comment: %s\n", sceneInfo->mComment.Buffer());

		KFbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
		if (thumbnail)
		{
			printf("    Thumbnail:\n");

			switch (thumbnail->GetDataFormat())
			{
			case KFbxThumbnail::eRGB_24:
				printf("        Format: RGB\n");
				break;
			case KFbxThumbnail::eRGBA_32:
				printf("        Format: RGBA\n");
				break;
			}

			switch (thumbnail->GetSize())
			{
			case KFbxThumbnail::eNOT_SET:
				printf("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
				break;
			case KFbxThumbnail::e64x64:
				printf("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
				break;
			case KFbxThumbnail::e128x128:
				printf("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
			}
		}
	}

}

