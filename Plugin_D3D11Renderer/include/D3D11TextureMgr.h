#ifndef __D3D11_TEXTUREMGR__HH
#define __D3D11_TEXTUREMGR__HH

#include "D3D11Prerequisites.h"
#include "TiTextureMgr.h"

namespace Titan
{
	class _D3D11DllExport D3D11TextureMgr : public TextureMgr
	{
	public:
		D3D11TextureMgr();

		~D3D11TextureMgr();

	protected:
		Resource* createImpl(const String& name, ResourceHandle id, const String& group, bool isManual, AnyMap* extraParams);
	};
}
#endif