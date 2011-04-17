#ifndef __TITAN_TEXTUREUNIT__HH
#define __TITAN_TEXTUREUNIT__HH

#include "TiPrerequisites.h"
#include "TiTexture.h"
#include "TitanCommon.h"

namespace Titan
{
	enum TexAddressMode
	{
		//Tile the texture at every integer junction. For example, for u values between
		//0 and 3, the texture is repeated three times; no mirroring is performed. 
		TAM_WARP = 0, 
		//Similar to TAM_WARP, except that the texture is flipped at every 
		//integer junction. For u values between 0 and 1, for example, the texture is 
		//addressed normally; between 1 and 2, the texture is flipped (mirrored); 
		//between 2 and 3, the texture is normal again; and so on. 
		TAM_MIRROR = 1,
		//Texture coordinates outside the range [0.0, 1.0] are set to the texture color
		//at 0.0 or 1.0, respectively. 
		TAM_CLAMP = 2,
		//Texture coordinates outside the range [0.0, 1.0] are set to the border color.
		TAM_BORDER = 3,

	};

	struct TexAddressModeSets
	{
		TexAddressMode	UTexAddrMode, VTexAddrMode, WTexAddrMode;
	};

	class _DllExport TextureUnit : public GeneralAlloc
	{
	public:
		TextureUnit();

		~TextureUnit();

		void			setTexture(TexturePtr tex){ mTexture = tex; }

		const TexturePtr&	getTexture() const { return mTexture; }

		void			_load();

		void			_unload();

		void			setMinFilter(FilterOptions fo){ mMinFilter = fo; }

		FilterOptions	getMinFilter() const { return mMinFilter ;}

		void			setMaxFilter(FilterOptions fo){ mMaxFilter = fo;}

		FilterOptions	getMaxFilter() const { return mMaxFilter; }

		void			setMipFilter(FilterOptions fo){ mMipFilter = fo; }

		FilterOptions	getMipFilter() const { return mMipFilter; }

		inline void		setTexAddressMode(TexAddressMode tam);

		const TexAddressModeSets&	getTexAddressModeSets() const { return mTexAddressModeSets;}

		inline void		setTexAddressMode(TexAddressMode uMode, TexAddressMode vMode, TexAddressMode wMode);

		void			setTexCoordSet(size_t tcs){ mTexCoordSet = tcs; }

		size_t			getTexCoordSet() const { return mTexCoordSet; }


	protected:
		TexturePtr			mTexture;
		FilterOptions		mMinFilter;
		FilterOptions		mMaxFilter;
		FilterOptions		mMipFilter;
		TexAddressModeSets	mTexAddressModeSets;
		size_t				mTexCoordSet;
	};
}
#endif