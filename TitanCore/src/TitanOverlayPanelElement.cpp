#include "TitanStableHeader.h"
#include "TitanOverlayPanelElement.h"
#include "TitanTextureMgr.h"
#include "VertexBuffer.h"
#include "HardwareBufferManager.h"
#include "Root.h"
#include "TitanRenderQueue.h"
#include "TitanOverlayMgr.h"


namespace Titan
{
	OverlayPanelElement::OverlayPanelElement(const String& name)
		:OverlayElement(name), mU1(0.0f), mV1(0.0f),
		mU2(1.0f), mV2(1.0f), mTexCoordNum(0)
	{
	}
	//-------------------------------------------------------------------------------//
	OverlayPanelElement::~OverlayPanelElement()
	{

	}
	//-------------------------------------------------------------------------------//
	void OverlayPanelElement::initialise(const String& texName, float width, float height, float left, float top)
	{
		mTexture = TextureMgr::getSingletonPtr()->load(texName);
		setSize(width, height);
		setPosition(left, top);
		mIsVisible = true;

		if(!mIsInitialised)
		{
			mRenderData.vertexData = TITAN_NEW VertexData();
			VertexDeclaration* decl = mRenderData.vertexData->vertexDecl;
			decl->addElement(0,0, VET_FLOAT3, VES_POSITION);

			mRenderData.vertexData->vertexStart = 0;
			mRenderData.vertexData->vertexCount = 4;

			VertexBufferSharedPtr vbuf = HardwareBufferManager::getSingletonPtr()->createVertexBuffer(decl->getVertexSize(0), mRenderData.vertexData->vertexCount,
				HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
			mRenderData.vertexData->vertexBufferBinding->setBinding(0, vbuf);

			mRenderData.useIndex = false;
			mRenderData.operationType = RenderData::OT_TRIANGLE_STRIP;

			mIsInitialised = true;
		}

		notifyGeometryOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayPanelElement::setUV(float u1, float v1, float u2, float v2)
	{
		mU1 = u1; mV1 = v1;
		mU2 = u2; mV2 = v2;
		notifyTexDataOld();
	}
	//-------------------------------------------------------------------------------//
	void OverlayPanelElement::updateGeometryData()
	{
		/*
			0-----2
			|    /|
			|  /  |
			|/    |
			1-----3
		*/
		Renderer* renderer = Root::getSingleton().getActiveRenderer();
		float offsetLeft = mLeft + renderer->getHorizontalTexelOffset() / OverlayMgr::getSingleton().getViewportWidth();
		float offsetTop = mTop + renderer->getVerticalTexelOffset() / OverlayMgr::getSingleton().getViewportHeight();
		float left, right, top, bottom;
		left = offsetLeft * 2 - 1;
		right = left + mWidth * 2;
		top = -((offsetTop * 2) - 1);
		bottom = top - mHeight * 2;

		VertexBufferSharedPtr vbuf = mRenderData.vertexData->vertexBufferBinding->getBuffer(0);

		float* pPos = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		float zValue = Root::getSingleton().getActiveRenderer()->getMaxDepthValue();
		*pPos++ = left;
		*pPos++ = top;
		*pPos++ = zValue;

		*pPos++ = left;
		*pPos++ = bottom;
		*pPos++ = zValue;

		*pPos++ = right;
		*pPos++ = top;
		*pPos++ = zValue;

		*pPos++ = right;
		*pPos++ = bottom;
		*pPos++ = zValue;

		vbuf->unlock();

		
	}
	//-------------------------------------------------------------------------------//
	void OverlayPanelElement::updateTexData()
	{
		if(!mTexture.isNull() &&mIsInitialised)
		{
			VertexDeclaration* decl = mRenderData.vertexData->vertexDecl;
			if(mTexCoordNum == 0)
			{
				decl->addElement(1, 0,VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

				VertexBufferSharedPtr vbuf = HardwareBufferManager::getSingletonPtr()->createVertexBuffer(decl->getVertexSize(1), mRenderData.vertexData->vertexCount,
					HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

				mRenderData.vertexData->vertexBufferBinding->setBinding(1,vbuf);
				mTexCoordNum = 1;
			}

			if(mTexCoordNum)
			{
				VertexBufferSharedPtr buf = mRenderData.vertexData->vertexBufferBinding->getBuffer(1);
				float* pVBStart = static_cast<float*>(buf->lock(HardwareBuffer::HBL_DISCARD));

				size_t uvSize = VertexElement::getTypeSize(VET_FLOAT2) / sizeof(float);
				size_t vertexSize = decl->getVertexSize(1) / sizeof(float);
				
				float* pTex = pVBStart;

				pTex[0] = mU1;	pTex[1] = mV1;
				pTex += vertexSize;

				pTex[0] = mU1;	pTex[1] = mV2;
				pTex += vertexSize;
				
				pTex[0] = mU2;	pTex[1] = mV1;
				pTex += vertexSize;

				pTex[0] = mU2;	pTex[1] = mV2;

				buf->unlock();
			}
			
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayPanelElement::updateRenderQueue(RenderQueue* queue)
	{
		if (mIsVisible)
		{
			if(!mTexture.isNull())
			{
				queue->addRenderable(this, RenderQueue::RGT_HUD, mZOrder);
			}
		}
	}
	//-------------------------------------------------------------------------------//





	//-------------------------------------------------------------------------------//
	OverlayPanelElementFactory::OverlayPanelElementFactory()
	{
		mTypeName = "OverlayPanelElement";
	}
	//-------------------------------------------------------------------------------//
	OverlayElement* OverlayPanelElementFactory::createInstance(const String& name)
	{
		return TITAN_NEW OverlayPanelElement(name);
	}
	//-------------------------------------------------------------------------------//
	void OverlayPanelElementFactory::destroyInstance(OverlayElement* element)
	{
		TITAN_DELETE element;
	}
}