#include "TerrainHost.h"
#include <windows.h>

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	BaseHost* pHost = TITAN_NEW TerrainHost();
	pHost->setup();
	pHost->Run();
	TITAN_DELETE pHost;

	return 0;
}