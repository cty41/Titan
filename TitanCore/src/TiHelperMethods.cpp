#include "TitanStableHeader.h"
#include "TiHelperMethods.h"

namespace Titan
{
	void MessagePump()
	{
		MSG  msg;
		while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

}