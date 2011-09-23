#include "TitanStableHeader.h"
#include "TiOverlayTextElement.h"
#include "TiFontMgr.h"
#include "TiVertexBuffer.h"
#include "TiHardwareBufferMgr.h"
#include "TiOverlayMgr.h"
#include "TiRenderQueue.h"
#include "TiRoot.h"
#include "TiRenderer.h"

namespace Titan
{
	#define DEFAULT_INITIAL_CHARS 12

	#define UNICODE_NEL 0x0085
	#define UNICODE_CR 0x000D
	#define UNICODE_LF 0x000A
	#define UNICODE_SPACE 0x0020
	#define UNICODE_ZERO 0x0030

	OverlayTextElement::OverlayTextElement(const String& name)
		:OverlayElement(name),
		mIsTansparent(false), mAlignment(TA_Left),
		mAllocSize(0), mCharHeight(0.02f),
		mPixelCharHeight(12), mSpaceWidth(0),
		mPixelSpaceWidth(0), mViewportAspectCoef(1),
		mColorTop(Color::White), mColorBottom(Color::White),
		mColorsChanged(true)
	{

	}
	//-------------------------------------------------------------------------------//
	OverlayTextElement::~OverlayTextElement()
	{
	
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::updateRenderQueue(RenderQueue* queue)
	{
		if (mIsVisible)
		{
			if(!mpFont->getMaterial().isNull())
			{
				queue->addRenderable(this, RGT_HUD, mZOrder);
			}
		}
	}
	//------------------------------------------------------------------------------//
	inline const MaterialPtr& OverlayTextElement::getMaterial() const
	{
		return mpFont->getMaterial();
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::initialise(const String& fontName, float left, float top, ushort pixelCharSize, TexAlignment ali)
	{
		setFontName(fontName);
		setPosition(left, top);
		setCharHeight(pixelCharSize);
		setTextAligmment(ali);

		if (!mIsInitialised)
		{
			// Set up the render op
			// Combine positions and texture coords since they tend to change together
			// since character sizes are different
			mRenderData.vertexData = TITAN_NEW VertexData();
			VertexDeclaration* decl = mRenderData.vertexData->vertexDecl;
			size_t offset = 0;
			// Positions
			decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
			offset += VertexElement::getTypeSize(VET_FLOAT3);
			// Texcoords
			decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
			offset += VertexElement::getTypeSize(VET_FLOAT2);
			// Colours - store these in a separate buffer because they change less often
			decl->addElement(1, 0, VET_COLOR, VES_DIFFUSE);

			mRenderData.operationType = OT_TRIANGLE_LIST;
			mRenderData.useIndex = false;
			mRenderData.vertexData->vertexStart = 0;
			// Vertex buffer will be created in checkMemoryAllocation

			checkMemoryAllocation( DEFAULT_INITIAL_CHARS );

			mIsInitialised = true;

		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::checkMemoryAllocation( size_t numChars )
	{
		if( mAllocSize < numChars)
		{
			// Create and bind new buffers
			// Note that old buffers will be deleted automatically through reference counting

			// 6 verts per char since we're doing tri lists without indexes
			// Allocate space for positions & texture coords
			VertexDeclaration* decl = mRenderData.vertexData->vertexDecl;
			VertexBufferBinding* bind = mRenderData.vertexData->vertexBufferBinding;

			mRenderData.vertexData->vertexCount = numChars * 6;

			// Create dynamic since text tends to change alot
			// positions & texcoords
			VertexBufferPtr vbuf = 
				HardwareBufferMgr::getSingleton().
				createVertexBuffer(
				decl->getVertexSize(0), 
				mRenderData.vertexData->vertexCount,
				HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
			bind->setBinding(0, vbuf);

			// colours
			vbuf = HardwareBufferMgr::getSingleton().
				createVertexBuffer(
				decl->getVertexSize(1), 
				mRenderData.vertexData->vertexCount,
				HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
			bind->setBinding(1, vbuf);

			mAllocSize = numChars;
			mColorsChanged = true;

		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::update()
	{
		float vpWidth, vpHeight;
		vpWidth = (float)OverlayMgr::getSingleton().getViewportWidth();
		vpHeight = (float)OverlayMgr::getSingleton().getViewportHeight();

		mViewportAspectCoef = vpHeight / vpWidth;

		if(mMetricsMode == OMM_ABSOLUTE)
		{
			if(OverlayMgr::getSingleton().hasViewportChanged() || mNeedUpdateGeometry)
			{
				// recalculate character size
				mCharHeight = (float) mPixelCharHeight / vpHeight;
				mSpaceWidth = (float) mPixelSpaceWidth / vpHeight;
				mNeedUpdateGeometry = true;
			}
		}

		OverlayElement::update();

		if(mColorsChanged && mIsInitialised)
		{
			updateColor();
			mColorsChanged;
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::updateGeometryData()
	{
		if(mpFont.isNull())
			return;

		size_t charlen = mCaption.size();
		checkMemoryAllocation(charlen);
		
		mRenderData.vertexData->vertexCount = charlen * 6;
		const VertexBufferPtr& vbuf = 
			mRenderData.vertexData->vertexBufferBinding->getBuffer(0);
		float *pVert;
		pVert = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		Renderer* renderer = Root::getSingleton().getActiveRenderer();
		float offsetLeft = mLeft + renderer->getHorizontalTexelOffset() / OverlayMgr::getSingleton().getViewportWidth();
		float offsetTop = mTop + renderer->getVerticalTexelOffset() / OverlayMgr::getSingleton().getViewportHeight();
		float largestWidth = 0;
		float left = offsetLeft * 2.0f - 1.0f;
		float top = -(offsetTop * 2.0f - 1.0f);

		if(mSpaceWidth == 0)
		{
			mSpaceWidth = mpFont->getGlyphAspectRatio(UNICODE_ZERO) * mCharHeight * 2.0f * mViewportAspectCoef;
		}

		String::iterator i = mCaption.begin(), iend = mCaption.end();
		bool newLine = true;
		for(; i != iend; ++i)
		{
			if (newLine)
			{
				float len = 0.0f;
				for(String::iterator j = i; j != iend; ++j)
				{
					uint32 character = (uint32)(*j);
					if (character == UNICODE_CR
						|| character == UNICODE_NEL
						|| character == UNICODE_LF) 
					{
						break;
					}
					else if (character == UNICODE_SPACE) // space
					{
						len += mSpaceWidth;
					}
					else 
					{
						len += mpFont->getGlyphAspectRatio(character) * mCharHeight * 2.0f * mViewportAspectCoef;
					}
				}

				if (mAlignment == TA_Right)
				{
					left -= len;
				}
				else if(mAlignment == TA_Center)
					left -= len * 0.5f;

				newLine = false;
			}

			uint32 character = (uint32)(*i);
			if (character == UNICODE_CR
				|| character == UNICODE_NEL
				|| character == UNICODE_LF)
			{
				left = offsetLeft * 2.0f - 1.0f;
				top -= mCharHeight * 2.0f;
				newLine = true;
				// Also reduce tri count
				mRenderData.vertexData->vertexCount -= 6;

				// consume CR/LF in one
				if (character == UNICODE_CR)
				{
					String::iterator peeki = i;
					peeki++;
					if (peeki != iend && (*peeki) == UNICODE_LF)
					{
						i = peeki; // skip both as one newline
						// Also reduce tri count
						mRenderData.vertexData->vertexCount -= 6;
					}

				}
				continue;
			}
			else if (character == UNICODE_SPACE) // space
			{
				// Just leave a gap, no tris
				left += mSpaceWidth;
				// Also reduce tri count
				mRenderData.vertexData->vertexCount -= 6;
				continue;
			}

			float horiz_height = mpFont->getGlyphAspectRatio(character) * mViewportAspectCoef ;
			const Font::UVRect& uvRect = mpFont->getGlyphTexCoords(character);

			// each vert is (x, y, z, u, v)
			//-------------------------------------------------------------------------------//----------------
			// First tri
			//
			// Upper left
			*pVert++ = left;
			*pVert++ = top;
			*pVert++ = -1.0;
			*pVert++ = uvRect.left;
			*pVert++ = uvRect.top;

			top -= mCharHeight * 2.0f;

			// Bottom left
			*pVert++ = left;
			*pVert++ = top;
			*pVert++ = -1.0;
			*pVert++ = uvRect.left;
			*pVert++ = uvRect.bottom;

			top += mCharHeight * 2.0f;
			left += horiz_height * mCharHeight * 2.0f;

			// Top right
			*pVert++ = left;
			*pVert++ = top;
			*pVert++ = -1.0;
			*pVert++ = uvRect.right;
			*pVert++ = uvRect.top;
			//-------------------------------------------------------------------------------//----------------

			//-------------------------------------------------------------------------------//----------------
			// Second tri
			//
			// Top right (again)
			*pVert++ = left;
			*pVert++ = top;
			*pVert++ = -1.0;
			*pVert++ = uvRect.right;
			*pVert++ = uvRect.top;

			top -= mCharHeight * 2.0f;
			left -= horiz_height  * mCharHeight * 2.0f;

			// Bottom left (again)
			*pVert++ = left;
			*pVert++ = top;
			*pVert++ = -1.0;
			*pVert++ = uvRect.left;
			*pVert++ = uvRect.bottom;

			left += horiz_height  * mCharHeight * 2.0f;

			// Bottom right
			*pVert++ = left;
			*pVert++ = top;
			*pVert++ = -1.0;
			*pVert++ = uvRect.right;
			*pVert++ = uvRect.bottom;
			//-------------------------------------------------------------------------------//----------------

			// Go back up with top
			top += mCharHeight * 2.0f;

			float currentWidth = (left + 1)/2 - offsetLeft;
			if (currentWidth > largestWidth)
			{
				largestWidth = currentWidth;

			}
		}
		vbuf->unlock();

		if (mMetricsMode == OMM_ABSOLUTE)
		{
			float vpWidth;
			vpWidth = (float) (OverlayMgr::getSingleton().getViewportWidth());

			largestWidth *= vpWidth;
		}
		if (getWidth() < largestWidth)
		{
			setWidth(largestWidth);
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::updateColor()
	{
		RGBA topColor, bottomColor;
		Root::getSingleton().convertColor(mColorTop, & topColor);
		Root::getSingleton().convertColor(mColorBottom, & bottomColor);

		VertexBufferPtr vbuf = mRenderData.vertexData->vertexBufferBinding->getBuffer(1);

		RGBA* pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		for(uint i = 0; i < mAllocSize; ++i)
		{
			// First tri (top, bottom, top)
			*pDest++ = topColor;
			*pDest++ = bottomColor;
			*pDest++ = topColor;
			// Second tri (top, bottom, bottom)
			*pDest++ = topColor;
			*pDest++ = bottomColor;
			*pDest++ = bottomColor;
		}
		vbuf->unlock();
	}
	//-------------------------------------------------------------------------------//
	inline void OverlayTextElement::setCaption(const String& content)
	{
		mCaption = content;
		mIsVisible = true;
		notifyGeometryOld();
		notifyTexDataOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setFontName(const String& name)
	{
		mpFont = FontMgr::getSingleton().load(name);
		if(mpFont.isNull())
			TITAN_EXCEPT_ITEMLOST(
			"could not find font" + name
			);
		
		notifyGeometryOld();
		notifyTexDataOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setCharHeight(float height)
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mPixelCharHeight = static_cast<ushort>(height);
		} 
		else
		{
			mCharHeight = height;
		}
		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setSpaceWidth( float width )
	{
		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mPixelSpaceWidth = static_cast<ushort>(width);
		} 
		else
		{
			mSpaceWidth = width;
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setColor(const Color& col)
	{
		mColorBottom = mColorTop = col;
		mColorsChanged = true;
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setColorBottom(const Color& col)
	{
		mColorBottom = col;
		mColorsChanged = true;
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setColorTop(const Color& col)
	{
		mColorTop = col;
		mColorsChanged;
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElement::setMetricsMode(OverlayMetricsMode mode)
	{
		float vpWidth, vpHeight;
		vpWidth = (float) (OverlayMgr::getSingleton().getViewportWidth());
		vpHeight = (float) (OverlayMgr::getSingleton().getViewportHeight());

		mViewportAspectCoef = vpHeight/vpWidth;

		OverlayElement::setMetricsMode(mode);

		if (mMetricsMode == OMM_ABSOLUTE)
		{
			mPixelCharHeight = static_cast<unsigned short>(mCharHeight * vpHeight);
			mPixelSpaceWidth = static_cast<unsigned short>(mSpaceWidth * vpHeight);
		} 

	}


	OverlayTextElementFactory::OverlayTextElementFactory()
	{
		mTypeName = "OverlayTextElement";
	}
	//-------------------------------------------------------------------------------//
	OverlayElement* OverlayTextElementFactory::createInstance(const String& name)
	{
		return TITAN_NEW OverlayTextElement(name);
	}
	//-------------------------------------------------------------------------------//
	void OverlayTextElementFactory::destroyInstance(OverlayElement* element)
	{
		TITAN_DELETE element;
	}
}