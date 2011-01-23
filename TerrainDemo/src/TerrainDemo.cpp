#include "BaseHost.h"
#include <windows.h>

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	BaseHost* pHost = TITAN_NEW BaseHost();
	pHost->setup();
	pHost->Run();
	TITAN_DELETE pHost;

	return 0;
}