#ifndef __TITAN_HELPERS__HH
#define __TITAN_HELPERS__HH

#include <assert.h>
#include <math.h>
#include "StlHeaders.h"


typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
typedef short int16;
typedef char int8;

typedef __int64   int64;
typedef unsigned __int64  uint64;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

class  Vector3
{
public:
	float x, y, z;

public:
	inline Vector3()
		:x(0.0f), y(0.0f), z(0.0f)
	{
	}

	inline Vector3( const float fX, const float fY, const float fZ )
		: x( fX ), y( fY ), z( fZ )
	{
	}

	inline explicit Vector3( const float afCoordinate[3] )
		: x( afCoordinate[0] ),
		y( afCoordinate[1] ),
		z( afCoordinate[2] )
	{
	}

	inline explicit Vector3( const int afCoordinate[3] )
	{
		x = (float)afCoordinate[0];
		y = (float)afCoordinate[1];
		z = (float)afCoordinate[2];
	}

	inline explicit Vector3( float* const r )
		: x( r[0] ), y( r[1] ), z( r[2] )
	{
	}

	inline explicit Vector3( const float scaler )
		: x( scaler )
		, y( scaler )
		, z( scaler )
	{
	}


	/** Exchange the contents of this vector with another. 
	*/
	inline void swap(Vector3& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}

	inline float operator [] ( const size_t i ) const
	{
		assert( i < 3 );

		return *(&x+i);
	}

	inline float& operator [] ( const size_t i )
	{
		assert( i < 3 );

		return *(&x+i);
	}
	/// Pointer accessor for direct copying
	inline float* ptr()
	{
		return &x;
	}
	/// Pointer accessor for direct copying
	inline const float* ptr() const
	{
		return &x;
	}

	/** Assigns the value of the other vector.
	@param
	rkVector The other vector
	*/
	inline Vector3& operator = ( const Vector3& rkVector )
	{
		x = rkVector.x;
		y = rkVector.y;
		z = rkVector.z;

		return *this;
	}

	inline Vector3& operator = ( const float fScaler )
	{
		x = fScaler;
		y = fScaler;
		z = fScaler;

		return *this;
	}

	inline bool operator == ( const Vector3& rkVector ) const
	{
		return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
	}

	inline bool operator != ( const Vector3& rkVector ) const
	{
		return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
	}

	// arithmetic operations
	inline Vector3 operator + ( const Vector3& rkVector ) const
	{
		return Vector3(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z);
	}

	inline Vector3 operator - ( const Vector3& rkVector ) const
	{
		return Vector3(
			x - rkVector.x,
			y - rkVector.y,
			z - rkVector.z);
	}

	inline Vector3 operator * ( const float fScalar ) const
	{
		return Vector3(
			x * fScalar,
			y * fScalar,
			z * fScalar);
	}

	inline Vector3 operator * ( const Vector3& rhs) const
	{
		return Vector3(
			x * rhs.x,
			y * rhs.y,
			z * rhs.z);
	}

	inline Vector3 operator / ( const float fScalar ) const
	{
		assert( fScalar != 0.0 );

		float fInv = 1.0f / fScalar;

		return Vector3(
			x * fInv,
			y * fInv,
			z * fInv);
	}

	inline Vector3 operator / ( const Vector3& rhs) const
	{
		return Vector3(
			x / rhs.x,
			y / rhs.y,
			z / rhs.z);
	}

	inline const Vector3& operator + () const
	{
		return *this;
	}

	inline Vector3 operator - () const
	{
		return Vector3(-x, -y, -z);
	}

	// overloaded operators to help Vector3
	inline friend Vector3 operator * ( const float fScalar, const Vector3& rkVector )
	{
		return Vector3(
			fScalar * rkVector.x,
			fScalar * rkVector.y,
			fScalar * rkVector.z);
	}

	inline friend Vector3 operator / ( const float fScalar, const Vector3& rkVector )
	{
		return Vector3(
			fScalar / rkVector.x,
			fScalar / rkVector.y,
			fScalar / rkVector.z);
	}

	inline friend Vector3 operator + (const Vector3& lhs, const float rhs)
	{
		return Vector3(
			lhs.x + rhs,
			lhs.y + rhs,
			lhs.z + rhs);
	}

	inline friend Vector3 operator + (const float lhs, const Vector3& rhs)
	{
		return Vector3(
			lhs + rhs.x,
			lhs + rhs.y,
			lhs + rhs.z);
	}

	inline friend Vector3 operator - (const Vector3& lhs, const float rhs)
	{
		return Vector3(
			lhs.x - rhs,
			lhs.y - rhs,
			lhs.z - rhs);
	}

	inline friend Vector3 operator - (const float lhs, const Vector3& rhs)
	{
		return Vector3(
			lhs - rhs.x,
			lhs - rhs.y,
			lhs - rhs.z);
	}

	// arithmetic updates
	inline Vector3& operator += ( const Vector3& rkVector )
	{
		x += rkVector.x;
		y += rkVector.y;
		z += rkVector.z;

		return *this;
	}

	inline Vector3& operator += ( const float fScalar )
	{
		x += fScalar;
		y += fScalar;
		z += fScalar;
		return *this;
	}

	inline Vector3& operator -= ( const Vector3& rkVector )
	{
		x -= rkVector.x;
		y -= rkVector.y;
		z -= rkVector.z;

		return *this;
	}

	inline Vector3& operator -= ( const float fScalar )
	{
		x -= fScalar;
		y -= fScalar;
		z -= fScalar;
		return *this;
	}

	inline Vector3& operator *= ( const float fScalar )
	{
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		return *this;
	}

	inline Vector3& operator *= ( const Vector3& rkVector )
	{
		x *= rkVector.x;
		y *= rkVector.y;
		z *= rkVector.z;

		return *this;
	}

	inline Vector3& operator /= ( const float fScalar )
	{
		assert( fScalar != 0.0 );

		float fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;
		z *= fInv;

		return *this;
	}

	inline Vector3& operator /= ( const Vector3& rkVector )
	{
		x /= rkVector.x;
		y /= rkVector.y;
		z /= rkVector.z;

		return *this;
	}

	inline float length () const
	{
		return sqrt( x * x + y * y + z * z );
	}

	inline float squaredLength () const
	{
		return x * x + y * y + z * z;
	}

	inline float distance(const Vector3& rhs) const
	{
		return (*this - rhs).length();
	}

	inline float squaredDistance(const Vector3& rhs) const
	{
		return (*this - rhs).squaredLength();
	}

	inline float dotProduct(const Vector3& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}


	inline float absDotProduct(const Vector3& vec) const
	{
		return fabs(x * vec.x) + fabs(y * vec.y) + fabs(z * vec.z);
	}


	inline float normalise()
	{
		float fLength = sqrt( x * x + y * y + z * z );

		// Will also work for zero-sized vectors, but will change nothing
		if ( fLength > 1e-08 )
		{
			float fInvLength = 1.0f / fLength;
			x *= fInvLength;
			y *= fInvLength;
			z *= fInvLength;
		}

		return fLength;
	}

	inline Vector3 crossProduct( const Vector3& rkVector ) const
	{
		return Vector3(
			y * rkVector.z - z * rkVector.y,
			z * rkVector.x - x * rkVector.z,
			x * rkVector.y - y * rkVector.x);
	}

	/** Returns a vector at a point half way between this and the passed
	in vector.
	*/
	inline Vector3 midPoint( const Vector3& vec ) const
	{
		return Vector3(
			( x + vec.x ) * 0.5f,
			( y + vec.y ) * 0.5f,
			( z + vec.z ) * 0.5f );
	}

	/** Returns true if the vector's scalar components are all greater
	that the ones of the vector it is compared against.
	*/
	inline bool operator < ( const Vector3& rhs ) const
	{
		if( x < rhs.x && y < rhs.y && z < rhs.z )
			return true;
		return false;
	}

	/** Returns true if the vector's scalar components are all smaller
	that the ones of the vector it is compared against.
	*/
	inline bool operator > ( const Vector3& rhs ) const
	{
		if( x > rhs.x && y > rhs.y && z > rhs.z )
			return true;
		return false;
	}

	inline void makeFloor( const Vector3& cmp )
	{
		if( cmp.x < x ) x = cmp.x;
		if( cmp.y < y ) y = cmp.y;
		if( cmp.z < z ) z = cmp.z;
	}

	inline void makeCeil( const Vector3& cmp )
	{
		if( cmp.x > x ) x = cmp.x;
		if( cmp.y > y ) y = cmp.y;
		if( cmp.z > z ) z = cmp.z;
	}

	/** Returns true if this vector is zero length. */
	inline bool isZeroLength(void) const
	{
		float sqlen = (x * x) + (y * y) + (z * z);
		return (sqlen < (1e-06 * 1e-06));

	}

};

#endif