#ifndef __TITAN_OVERLAY_TEXTELEMENT__HH
#define __TITAN_OVERLAY_TEXTELEMENT__HH

#include "TitanPrerequisites.h"
#include "TitanOverlayElement.h"
#include "TitanFont.h"

namespace Titan
{
	class _DllExport OverlayTextElement : public OverlayElement
	{
	public:
		enum TexAlignment
		{
			TA_Left,
			TA_Right,
			TA_Center,
		};
	public:
		OverlayTextElement(const String& name);

		~OverlayTextElement();

		void updateRenderQueue(RenderQueue* queue);

		//own methods
		void initialise(const String& fontName, float left, float top, ushort pixelCharSize = 12, TexAlignment ali = TA_Left);

		void setCaption(const String& content);

		const String& getCaption() const { return mCaption; }

		void setFontName(const String& name);

		const String& getFontName() const { return mpFont->getName(); }

		const FontPtr& getFont() const { return mpFont; }

		void setCharHeight(float height);

		float getCharHeight() const 
		{ 
			if(mMetricsMode == OMM_ABSOLUTE)
				return mPixelCharHeight;
			else
				return mCharHeight; 
		}

		void setSpaceWidth( float width );
		
		float getSpaceWidth() const
		{ 
			if(mMetricsMode == OMM_ABSOLUTE)
				return mPixelSpaceWidth;
			else
				return mSpaceWidth; 
		}

		void setColor(const Color& col);

		const Color& getColor(void) const { return mColorTop; }

		 void setColorBottom(const Color& col);

		 const Color& getColorBottom(void) const{ return mColorBottom; }

        void setColorTop(const Color& col);

		const Color& getColorTop(void) const { return mColorTop; }

		void setTextAligmment(TexAlignment a)
		{
			mAlignment = a;
			notifyGeometryOld();
		}

		TexAlignment getTextAlignment() const { return mAlignment; }

		//OverlayElement methods
		void	update();

		void	setMetricsMode(OverlayMetricsMode mode);

		//renderable methods
		RenderData*		getRenderData() { return &mRenderData; }

		void			getRenderData(RenderData& data){ data = mRenderData; }

		const TexturePtr& getTexture() const { return mpFont->getFontTexture(); }

	protected:

		//OverlayElement methods
		void updateGeometryData();

		void updateTexData(){};

		void updateColor();

		void checkMemoryAllocation( size_t numChars );

	protected:
		FontPtr			mpFont;
		String			mCaption;
		TexAlignment	mAlignment;

		bool			mIsTansparent;
		RenderData		mRenderData;

		float			mCharHeight;
		ushort			mPixelCharHeight;
		float			mSpaceWidth;
		ushort			mPixelSpaceWidth;
		size_t			mAllocSize;
		float			mViewportAspectCoef;

		Color			mColorBottom;
		Color			mColorTop;
		bool			mColorsChanged;

	};

	class _DllExport OverlayTextElementFactory : public OverlayElementFactory
	{
	public:
		OverlayTextElementFactory();

		~OverlayTextElementFactory(){};

		OverlayElement* createInstance(const String& name);

		void  destroyInstance(OverlayElement* element);

	};
}

#endif