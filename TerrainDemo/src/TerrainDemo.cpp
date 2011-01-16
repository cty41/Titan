#include "BaseHost.h"
#include <windows.h>

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	BaseHost* pHost = new BaseHost();
	pHost->setup();
	pHost->Run();
	delete pHost;

	return 0;
}