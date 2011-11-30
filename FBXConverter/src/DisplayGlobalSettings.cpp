#include <fbxsdk.h>

#include "DisplayCommon.h"



void DisplayGlobalTimeSettings(KFbxGlobalSettings* pGlobalSettings)
{
	char lTimeString[256];

	DisplayString("Time Mode : ", KTime_GetTimeModeName(pGlobalSettings->GetTimeMode()));

	KTimeSpan lTs;
	KTime     lStart, lEnd;
	pGlobalSettings->GetTimelineDefaultTimeSpan(lTs);
	lStart = lTs.GetStart();
	lEnd   = lTs.GetStop();
	DisplayString("Timeline default timespan: ");
	DisplayString("     Start: ", lStart.GetTimeString(lTimeString));
	DisplayString("     Stop : ", lEnd.GetTimeString(lTimeString));

	DisplayString("");
}
