#ifndef _D3D9_TEXTUREMGR_HH
#define _D3D9_TEXTUREMGR_HH

#include "D3D9Prerequisites.h"
#include "TiTextureMgr.h"

namespace Titan
{
	class _D3D9DllExport D3D9TextureMgr : public TextureMgr
	{
	public:
		D3D9TextureMgr();

		~D3D9TextureMgr();

	protected:
		Resource* createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams);


	};
}

#endif