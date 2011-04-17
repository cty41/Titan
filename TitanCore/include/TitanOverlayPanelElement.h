#ifndef __TITAN_OVERLAY_PANELELEMENT__HH
#define __TITAN_OVERLAY_PANELELEMENT__HH

#include "TiPrerequisites.h"
#include "TitanOverlayElement.h"
#include "TiRenderData.h"
#include "TiTexture.h"

namespace Titan
{
	class _DllExport OverlayPanelElement : public OverlayElement
	{
	public:
		OverlayPanelElement(const String& name);

		~OverlayPanelElement();

		void initialise(const String& texName, float width, float height, float left, float top);

		void setUV(float u1, float v1, float u2, float v2);

		//overlay method
		void updateRenderQueue(RenderQueue* queue);

		//renderable methods
		RenderData*		getRenderData() { return &mRenderData; }

		void			getRenderData(RenderData& data){ data = mRenderData; }

		const TexturePtr& getTexture() const { return mTexture; }
		
	
	protected:
		//overlay methods
		void updateGeometryData();

		void updateTexData();

	protected:
		float	mU1, mV1, mU2, mV2;
		uint	mTexCoordNum;

		RenderData	mRenderData;
		TexturePtr	mTexture;

	};

	class _DllExport OverlayPanelElementFactory : public OverlayElementFactory
	{
	public:
		OverlayPanelElementFactory();

		~OverlayPanelElementFactory(){};

		OverlayElement* createInstance(const String& name);

		void  destroyInstance(OverlayElement* element);
	};
}

#endif