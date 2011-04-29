#ifndef __TITAN_TEXTUREUNIT__HH
#define __TITAN_TEXTUREUNIT__HH

#include "TiPrerequisites.h"
#include "TiTexture.h"
#include "TiCommon.h"
#include "TiMatrix4.h"
#include "TiReflector.h"

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

	class _DllExport TextureUnit : public Reflector, public GeneralAlloc
	{
	public:
		TextureUnit(Pass* parent);

		~TextureUnit();

		void			setTexture(const String&  name, TexType type = TT_2D);

		const TexturePtr&	getTexture();

		void			_load();

		void			_unload();

		void			setMinFilter(FilterOptions fo){ mMinFilter = fo; }

		FilterOptions	getMinFilter() const { return mMinFilter ;}

		void			setMagFilter(FilterOptions fo){ mMagFilter = fo;}

		FilterOptions	getMagFilter() const { return mMagFilter; }

		void			setMipFilter(FilterOptions fo){ mMipFilter = fo; }

		FilterOptions	getMipFilter() const { return mMipFilter; }

		inline void		setTexAddressMode(TexAddressMode tam);

		const TexAddressModeSets&	getTexAddressModeSets() const { return mTexAddressModeSets;}

		inline void		setTexAddressMode(TexAddressMode uMode, TexAddressMode vMode, TexAddressMode wMode);

		void			setTexCoordSet(size_t tcs){ mTexCoordSet = tcs; }

		size_t			getTexCoordSet() const { return mTexCoordSet; }

		const Matrix4&	getTextureTransform();

		void			setTextureTransform(const Matrix4& transform);

		void			recalcTextureMatrix();


	protected:
		Pass*				mParent;
		String				mTexName;
		TexType				mType;
		TexturePtr			mTexture;
		FilterOptions		mMinFilter;
		FilterOptions		mMagFilter;
		FilterOptions		mMipFilter;
		TexAddressModeSets	mTexAddressModeSets;
		size_t				mTexCoordSet;

		bool mRecalcTexMatrix;
		Matrix4 mTexMatrix;

	protected:
		virtual void setupParamsCmd();

		class _DllExport MinFilterCmd : public ParamsCommand
		{
		public:
			virtual void setter(void* target, const String& val);

			virtual String getter(void* target);
		};

		class _DllExport MagFilterCmd : public ParamsCommand
		{
		public:
			virtual void setter(void* target, const String& val);

			virtual String getter(void* target);
		};

		class _DllExport MipFilterCmd : public ParamsCommand
		{
		public:
			virtual void setter(void* target, const String& val);

			virtual String getter(void* target);
		};

		class _DllExport TexAddressModeCmd : public ParamsCommand
		{
		public:
			virtual void setter(void* target, const String& val);

			virtual String getter(void* target);
		};

		class _DllExport TexCoordSetCmd : public ParamsCommand
		{
		public:
			virtual void setter(void* target, const String& val);

			virtual String getter(void* target);
		};

		static FilterOptions	mapToFilterOptions(const String&  val);

		static String	FilterOptionsToString(FilterOptions fo);
		
		static MinFilterCmd	msMinFilterCmd;
		static MagFilterCmd	msMagFilterCmd;
		static MipFilterCmd	msMipFilterCmd;
		static TexAddressModeCmd msTexAddressModeCmd;
		static TexCoordSetCmd	msTexCoordSetCmd;
	};
}
#endif