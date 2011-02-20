#ifndef __TITAN_TEXTURE_HH
#define __TITAN_TEXTURE_HH

#include "TitanPrerequisites.h"
#include "TitanResource.h"
#include "TitanCommon.h"

namespace Titan
{
	enum TexType
	{
		TT_1D	= 0,
		TT_2D	= 1,
		TT_3D	= 2
	};

	class _DllExport Texture: public Resource
	{
	public:
		Texture(ResourceMgr* mgr,const String& name, ResourceHandle id, const String& group);

		~Texture();

		
	protected:
		PixelFormat		mPixelFormat;
		TexType			mType;

	};
}
#endif
