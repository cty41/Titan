#ifndef __TITAN_OVERLAY_ELEMENT__HH
#define __TITAN_OVERLAY_ELEMENT__HH

#include "TiPrerequisites.h"
#include "TiRenderable.h"

namespace Titan
{
	enum OverlayMetricsMode
	{
		OMM_RELATIVE,
		OMM_ABSOLUTE,
	};

	class _DllExport OverlayElement : public Renderable
	{
	public:
		OverlayElement(const String& name);

		virtual ~OverlayElement(){};

		void	setName(const String& name);

		void	setZOrder(ushort zOrder){ mZOrder = zOrder;}

		void	setWidth(float width);

		void	setHeight(float height);

		void	setLeft(float left);

		void	setTop(float top);

		void	setVisible(bool state){ mIsVisible = state;}

		const String& getName() const { return mName; }

		ushort	getZOrder() const { return mZOrder; }

		float	getWidth() const 
		{ 
			if (mMetricsMode == OMM_ABSOLUTE)
				return mAbsWidth;
			else
				return mWidth; 
		}

		float	getHeight() const
		{ 
			if (mMetricsMode == OMM_ABSOLUTE)
				return mAbsHeight;
			else
				return mHeight; 
		}

		float	getLeft() const 
		{ 
			if (mMetricsMode == OMM_ABSOLUTE)
				return mAbsLeft;
			else
				return mLeft; 
		}

		float	getTop() const 
		{ 
			if (mMetricsMode == OMM_ABSOLUTE)
				return mAbsTop;
			else
				return mTop; 
		}

		bool	isVisible() const { return mIsVisible; }

		void	show() { mIsVisible = true; }

		void	hide() { mIsVisible = false; }

		void	setSize(float width, float height);

		void	setPosition(float left, float top);

		virtual void	setMetricsMode(OverlayMetricsMode mode);

		OverlayMetricsMode	getMetricsMode() const { return mMetricsMode; }

		virtual void update();

		virtual void updateRenderQueue(RenderQueue* queue) = 0;

		void	notifyGeometryOld(){ mNeedUpdateGeometry = true;}

		void	notifyTexDataOld(){ mNeedUpdateTexData = true;}

		void	getTransformMat(Matrix4* transMat);

		const MaterialPtr&	getMaterial() const { return mMaterial; }

		float					getSquaredDistance(Camera* cam);

		virtual bool			IsUseIdentityView() const {return true;}

		virtual bool			IsUseIdentityProj() const { return true;}

	protected:
		virtual void updateGeometryData() = 0;

		virtual void updateTexData() = 0;

	protected:
		String	mName;
		MaterialPtr	mMaterial;

		OverlayMetricsMode	mMetricsMode;
		ushort	mZOrder;
		float	mWidth;
		float	mHeight;
		float	mLeft;
		float	mTop;
		float	mAbsWidth;
		float	mAbsHeight;
		float	mAbsLeft;
		float	mAbsTop;
		float	mPixelScaleX;
		float	mPixelScaleY;
		bool	mIsInitialised;
		bool	mIsVisible;
		bool	mNeedUpdateGeometry;
		bool	mNeedUpdateTexData;
	};


	class _DllExport OverlayElementFactory : public GeneralAlloc
	{
	public:
		OverlayElementFactory(){}

		virtual ~OverlayElementFactory(){}

		const String& getTypeName() const { return mTypeName; }

		virtual OverlayElement* createInstance(const String& name) = 0;

		virtual void		destroyInstance(OverlayElement* element) = 0;
	protected:
		String		mTypeName;
	};
}

#endif