#ifndef __TITAN__RECT_2D__HH
#define __TITAN__RECT_2D__HH


#include "TitanPrerequisites.h"
#include "TitanVector2.h"
#include "TitanNumericTools.h"

namespace Titan
{
	class _DllExport Rect2D
	{
	public:
		float x0, x1;
		float y0, y1;

		Rect2D(){};
		Rect2D(float _x0, float _x1, float _y0, float _y1);
		Rect2D(const Rect2D& src);
		Rect2D(float _xSize, float _ySize);
		explicit Rect2D(const Vector2& size);
		~Rect2D(){};

		// negate
		Rect2D operator - ();

		// equality
		bool operator == (const Rect2D& src)const; 

		// inequality
		bool operator != (const Rect2D& src)const; 

		// copy
		const Rect2D& operator = (const Rect2D& src); 

		const Rect2D& operator = (const Vector2& vec); 

		const Rect2D& operator += (const Vector2& vec); 
		const Rect2D& operator += (float s);

		const Rect2D& operator -= (const Vector2& vec); 
		const Rect2D& operator -= (float s);

		const Rect2D& operator *= (const Vector2& vec) ;
		const Rect2D& operator *= (float s) ;

		const Rect2D& operator /= (const Vector2& vec) ;		
		const Rect2D& operator /= (float s) ;

		void clear();
		bool isClear()const;
		bool valid()const;
		void normalize();

		void set(const Rect2D& v);
		void set(float _x0, float _x1, float _y0, float _y1); 
		void set(const Vector2& size); 

		void setFloor(const Rect2D& input);
		void setCeiling(const Rect2D& input);

		void resizeX(float span);
		void resizeY(float span);
		void resize(const Vector2& size);

		void resizeMinX(float span);
		void resizeMinY(float span);
		void resizeMinZ(float span);
		void resizeMin(const Vector2& size);

		void resizeMaxX(float span);
		void resizeMaxY(float span);
		void resizeMaxZ(float span);
		void resizeMax(const Vector2& size);

		float sizeX()const;
		float sizeY()const;
		Vector2 size()const;

		float midX()const;
		float midY()const;
		Vector2 midpoint()const;
		Vector2 minPoint()const;
		Vector2 maxPoint()const;

		float area()const;

		void unionPoint(const Vector2& point);
		void unionRect(const Rect2D& rect);

		void offset(const Vector2 &offset);

		void expand(float n);
		void expand(const Vector2& size);

		void expandX(float n);
		void expandY(float n);

		Vector2 corner(int index)const;

		bool pointInRect(const Vector2& point)const;
	};


	inline Rect2D::Rect2D(float _x0, float _y0, float _x1, float _y1)
		:x0(_x0)
		,x1(_x1)
		,y0(_y0)
		,y1(_y1){};


	inline Rect2D::Rect2D(const Rect2D& src)
		:x0(src.x0)
		,x1(src.x1)
		,y0(src.y0)
		,y1(src.y1){};


	inline Rect2D::Rect2D(float _xSize, float _ySize)
		:x0(0.0f)
		,x1(0.0f)
		,y0(0.0f)
		,y1(0.0f)
	{
		resize(Vector2(_xSize, _ySize));
	}


	inline Rect2D::Rect2D(const Vector2& size)
		:x0(0.0f)
		,x1(0.0f)
		,y0(0.0f)
		,y1(0.0f)
	{
		resize(size);
	}

	//--------------------------------------------------
	// Operators
	//

	inline bool Rect2D::operator == (const Rect2D& src)const
	{
		return (	x0 == src.x0 
			&&  x1 == src.x1
			&&  y0 == src.y0
			&&  y1 == src.y1);
	}



	inline bool Rect2D::operator != (const Rect2D& src)const
	{
		return (	x0 != src.x0 
			||  x1 != src.x1
			||  y0 != src.y0
			||  y1 != src.y1);
	}



	inline const Rect2D& Rect2D::operator = (const Rect2D& src)
	{
		x0 = src.x0; 
		x1 = src.x1;
		y0 = src.y0;
		y1 = src.y1;
		return(*this);
	}



	inline const Rect2D& Rect2D::operator += (const Vector2& vec)
	{
		x0 += vec.x; 
		x1 += vec.x; 
		y0 += vec.y; 
		y1 += vec.y; 
		return(*this);
	}


	inline const Rect2D& Rect2D::operator += (float s)
	{
		x0 += s; 
		x1 += s; 
		y0 += s; 
		y1 += s; 
		return(*this);
	}


	inline Rect2D operator+(const Rect2D& lha, const Vector2& rha)
	{
		return(Rect2D(	lha.x0 + rha.x,
			lha.x1 + rha.x,
			lha.y0 + rha.y,
			lha.y1 + rha.y));
	}


	inline Rect2D operator+(const Vector2& lha, const Rect2D& rha)
	{
		return(Rect2D(	rha.x0 + lha.x,
			rha.x1 + lha.x,
			rha.y0 + lha.y,
			rha.y1 + lha.y));
	}


	inline Rect2D operator+(const Rect2D& lha, float rha)
	{
		return(Rect2D(	lha.x0 + rha,
			lha.x1 + rha,
			lha.y0 + rha,
			lha.y1 + rha));
	}


	inline Rect2D operator+(float lha, const Rect2D& rha)
	{
		return(Rect2D(	rha.x0 + lha,
			rha.x1 + lha,
			rha.y0 + lha,
			rha.y1 + lha));
	}


	inline const Rect2D& Rect2D::operator-=(const Vector2& vec)
	{
		x0 -= vec.x; 
		x1 -= vec.x; 
		y0 -= vec.y; 
		y1 -= vec.y; 
		return(*this);
	}


	inline const Rect2D& Rect2D::operator-= (float s)
	{
		x0 -= s; 
		x1 -= s; 
		y0 -= s; 
		y1 -= s; 
		return(*this);
	}



	inline Rect2D operator - (const Rect2D& lha, const Vector2& rha)
	{
		return(Rect2D(	lha.x0 - rha.x,
			lha.x1 - rha.x,
			lha.y0 - rha.y,
			lha.y1 - rha.y));
	}



	inline Rect2D operator - (const Rect2D& lha, float rha)
	{
		return(Rect2D(	lha.x0 - rha,
			lha.x1 - rha,
			lha.y0 - rha,
			lha.y1 - rha));
	}


	inline Rect2D Rect2D::operator - ()
	{
		return(Rect2D(-x1,-x0,-y1,-y0));
	}


	inline const Rect2D& Rect2D::operator *= (float s)
	{
		x0 *= s; 
		x1 *= s; 
		y0 *= s; 
		y1 *= s; 
		return(*this);
	}


	inline const Rect2D& Rect2D::operator *= (const Vector2& vec)
	{
		x0 *= vec.x; 
		x1 *= vec.x; 
		y0 *= vec.y; 
		y1 *= vec.y; 
		return(*this);
	}



	inline Rect2D operator * (const Rect2D& lha, const Vector2& rha)
	{
		return(Rect2D(	lha.x0 * rha.x,
			lha.x1 * rha.x,
			lha.y0 * rha.y,
			lha.y1 * rha.y));
	}

	inline Rect2D operator * (const Vector2& lha, const Rect2D& rha)
	{
		return(Rect2D(	rha.x0 * lha.x,
			rha.x1 * lha.x,
			rha.y0 * lha.y,
			rha.y1 * lha.y));
	}



	inline Rect2D operator * (const Rect2D& lha, float rha)
	{
		return(Rect2D(	lha.x0 * rha,
			lha.x1 * rha,
			lha.y0 * rha,
			lha.y1 * rha));
	}


	inline Rect2D operator * (float lha, const Rect2D& rha)
	{
		return(Rect2D(	rha.x0 * lha,
			rha.x1 * lha,
			rha.y0 * lha,
			rha.y1 * lha));
	}


	inline const Rect2D& Rect2D::operator /= (float s)
	{
		assert(s != 0.0f&&"divide by zero error");
		x0 /= s; 
		x1 /= s; 
		y0 /= s; 
		y1 /= s; 
		return(*this);
	}


	inline const Rect2D& Rect2D::operator /= (const Vector2& vec)
	{
		assert(vec.x != 0.0f&& "divide by zero error");
		assert(vec.y != 0.0f&& "divide by zero error");

		x0 /= vec.x; 
		x1 /= vec.x; 
		y0 /= vec.y; 
		y1 /= vec.y; 
		return(*this);
	}


	inline Rect2D operator / (const Rect2D& lha, const Vector2& rha)
	{
		assert(rha.x != 0.0f&& "divide by zero error");
		assert(rha.y != 0.0f&& "divide by zero error");

		return(Rect2D(	lha.x0 / rha.x,
			lha.x1 / rha.x,
			lha.y0 / rha.y,
			lha.y1 / rha.y));
	}

	inline Rect2D operator / (const Rect2D& lha, float rha)
	{
		assert(rha != 0.0f&& "divide by zero error");

		return(Rect2D(	lha.x0 / rha,
			lha.x1 / rha,
			lha.y0 / rha,
			lha.y1 / rha));
	}

	//--------------------------------------------------
	// Inline Functions
	//

	inline void Rect2D::clear(void)
	{
		x0 = 0.0f; 
		x1 = 0.0f; 
		y0 = 0.0f; 
		y1 = 0.0f; 
	}


	inline bool Rect2D::isClear(void)const
	{
		return ((0.0f == x0)
			&& (0.0f == x1)
			&& (0.0f == y0)
			&& (0.0f == y1));
	}


	inline void Rect2D::set (const Rect2D& src)
	{
		x0 = src.x0; 
		x1 = src.x1;
		y0 = src.y0;
		y1 = src.y1;
	}


	inline void Rect2D::set (float _x0, float _x1, float _y0, float _y1)
	{
		x0 = _x0; 
		x1 = _x1;
		y0 = _y0;
		y1 = _y1;
	}


	inline void Rect2D::set (const Vector2& size)
	{
		clear();
		resize(size);
	}


	//
	// setFloor
	//

	inline void Rect2D::setFloor(const Rect2D& input)
	{
		x0 = floorf(input.x0);
		x1 = floorf(input.x1);
		y0 = floorf(input.y0);
		y1 = floorf(input.y1);
	}

	//
	// setCeiling
	//
	inline void Rect2D::setCeiling(const Rect2D& input)
	{
		x0 = ceilf(input.x0);
		x1 = ceilf(input.x1);
		y0 = ceilf(input.y0);
		y1 = ceilf(input.y1);
	}


	//
	// valid()
	//

	inline bool Rect2D::valid()const
	{
		return(		x0 <= x1
			&&	y0 <= y1);
	}

	inline void Rect2D::resizeX(float size)
	{
		x1 = midX() + (size*0.5f);
		x0 = x1 - size;
	}


	inline void Rect2D::resizeY(float size)
	{
		y1 = midY() + (size*0.5f);
		y0 = y1 - size;
	}



	inline void Rect2D::resize(const Vector2& size)
	{
		resizeX(size.x);
		resizeY(size.y);
	}



	inline void Rect2D::resizeMaxX(float span)
	{
		x1 = x0 + span;
	}


	inline void Rect2D::resizeMaxY(float span)
	{
		y1 = y0 + span;
	}


	inline void Rect2D::resizeMax(const Vector2& size)
	{
		resizeMaxX(size.x);
		resizeMaxY(size.y);
	}



	inline void Rect2D::resizeMinX(float span)
	{
		x0 = x1 - span;
	}


	inline void Rect2D::resizeMinY(float span)
	{
		y0 = y1 - span;
	}


	inline void Rect2D::resizeMin(const Vector2& size)
	{
		resizeMinX(size.x);
		resizeMinY(size.y);
	}


	inline float Rect2D::midX()const
	{
		return (x0+x1)*0.5f;
	}


	inline float Rect2D::midY()const
	{
		return (y0+y1)*0.5f;
	}


	inline Vector2 Rect2D::midpoint()const
	{
		return Vector2(midX(), midY());
	}


	inline float Rect2D::sizeX()const
	{
		return (x1-x0);
	}


	inline float Rect2D::sizeY()const
	{
		return (y1-y0);
	}


	inline Vector2 Rect2D::size()const
	{
		return Vector2(sizeX(), sizeY());
	}


	inline Vector2 Rect2D::minPoint()const
	{
		return Vector2(x0, y0);
	}


	inline Vector2 Rect2D::maxPoint()const
	{
		return Vector2(x1, y1);
	}


	inline float Rect2D::area()const
	{
		return(sizeX()*sizeY());
	}


	inline void Rect2D::unionPoint(const Vector2& point)
	{
		x0 = minimum(x0, point.x);
		y0 = minimum(y0, point.y);

		x1 = maximum(x1, point.x);
		y1 = maximum(y1, point.y);
	}


	inline void Rect2D::unionRect(const Rect2D& rect)
	{
		x0 = minimum(x0, rect.x0);
		y0 = minimum(y0, rect.y0);

		x1 = maximum(x1, rect.x1);
		y1 = maximum(y1, rect.y1);
	}


	inline void Rect2D::offset(const Vector2 &offset)
	{
		x0 += offset.x;
		x1 += offset.x;
		y0 += offset.y;
		y1 += offset.y;
	}



	inline void Rect2D::expand(float n)
	{
		x0 -= n;
		x1 += n;
		y0 -= n;
		y1 += n;
	}


	inline void Rect2D::expandX(float n)
	{
		x0 -= n;
		x1 += n;
	}


	inline void Rect2D::expandY(float n)
	{
		y0 -= n;
		y1 += n;
	}


	inline void Rect2D::expand(const Vector2& size)
	{
		expandX(size.x);
		expandY(size.y);
	}



	inline void Rect2D::normalize()
	{
		if(x0 > x1)
		{
			swap(x0, x1);
		}
		if(y0 > y1)
		{
			swap(y0, y1);
		}
	}



	inline Vector2 Rect2D::corner(int index)const
	{
		assert(index>=0 && index <4&& "invalid index");

		return Vector2( (index & 1) ? x0: x1,
			(index & 2) ? y0: y1);
	}

	inline bool Rect2D::pointInRect(const Vector2& point)const
	{
		return point.x >= x0
			&& point.y >= y0
			&& point.x <= x1
			&& point.y <= y1;
	}

}


#endif