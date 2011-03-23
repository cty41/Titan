//completely copied from ogre
#ifndef _TITAN_COLOR_HH
#define _TITAN_COLOR_HH

#include "TitanPrerequisites.h"

namespace Titan
{
	typedef uint32 RGBA;
    typedef uint32 ARGB;
    typedef uint32 ABGR;
	typedef uint32 BGRA;

    /** Class representing colour.
	    @remarks
		    Colour is represented as 4 components, each of which is a
		    floating-point value from 0.0 to 1.0.
	    @par
		    The 3 'normal' colour components are red, green and blue, a higher
		    number indicating greater amounts of that component in the colour.
		    The forth component is the 'alpha' value, which represents
		    transparency. In this case, 0.0 is completely transparent and 1.0 is
		    fully opaque.
    */
    class _DllExport Color
    {
    public:
        static const Color ZERO;
        static const Color Black;
        static const Color White;
		static const Color Grey;
        static const Color Red;
        static const Color Green;
        static const Color Blue;

	    explicit Color( float red = 1.0f,
				    float green = 1.0f,
				    float blue = 1.0f,
				    float alpha = 1.0f ) : r(red), g(green), b(blue), a(alpha)
        { }

	    bool operator==(const Color& rhs) const;
	    bool operator!=(const Color& rhs) const;

        float r,g,b,a;

	    /** Retrieves colour as RGBA.
	    */
	    RGBA getAsRGBA(void) const;

	    /** Retrieves colour as ARGB.
	    */
	    ARGB getAsARGB(void) const;

		/** Retrieves colour as BGRA.
		*/
		BGRA getAsBGRA(void) const;

		/** Retrieves colours as ABGR */
	    ABGR getAsABGR(void) const;

	    /** Sets colour as RGBA.
	    */
        void setAsRGBA(const RGBA val);

	    /** Sets colour as ARGB.
	    */
        void setAsARGB(const ARGB val);

		/** Sets colour as BGRA.
		*/
		void setAsBGRA(const BGRA val);

	    /** Sets colour as ABGR.
	    */
        void setAsABGR(const ABGR val);

        /** Clamps colour value to the range [0, 1].
        */
        void saturate(void)
        {
            if (r < 0)
                r = 0;
            else if (r > 1)
                r = 1;

            if (g < 0)
                g = 0;
            else if (g > 1)
                g = 1;

            if (b < 0)
                b = 0;
            else if (b > 1)
                b = 1;

            if (a < 0)
                a = 0;
            else if (a > 1)
                a = 1;
        }

        /** As saturate, except that this colour value is unaffected and
            the saturated colour value is returned as a copy. */
        Color saturateCopy(void) const
        {
            Color ret = *this;
            ret.saturate();
            return ret;
        }

		/// Array accessor operator
		inline float operator [] ( const size_t i ) const
		{
			assert( i < 4 );

			return *(&r+i);
		}

		/// Array accessor operator
		inline float& operator [] ( const size_t i )
		{
			assert( i < 4 );

			return *(&r+i);
		}

		/// Pointer accessor for direct copying
		inline float* ptr()
		{
			return &r;
		}
		/// Pointer accessor for direct copying
		inline const float* ptr() const
		{
			return &r;
		}

		
		// arithmetic operations
        inline Color operator + ( const Color& rkVector ) const
        {
            Color kSum;

            kSum.r = r + rkVector.r;
            kSum.g = g + rkVector.g;
            kSum.b = b + rkVector.b;
            kSum.a = a + rkVector.a;

            return kSum;
        }

        inline Color operator - ( const Color& rkVector ) const
        {
            Color kDiff;

            kDiff.r = r - rkVector.r;
            kDiff.g = g - rkVector.g;
            kDiff.b = b - rkVector.b;
            kDiff.a = a - rkVector.a;

            return kDiff;
        }

        inline Color operator * (const float fScalar ) const
        {
            Color kProd;

            kProd.r = fScalar*r;
            kProd.g = fScalar*g;
            kProd.b = fScalar*b;
            kProd.a = fScalar*a;

            return kProd;
        }

        inline Color operator * ( const Color& rhs) const
        {
            Color kProd;

            kProd.r = rhs.r * r;
            kProd.g = rhs.g * g;
            kProd.b = rhs.b * b;
            kProd.a = rhs.a * a;

            return kProd;
        }

        inline Color operator / ( const Color& rhs) const
        {
            Color kProd;

            kProd.r = rhs.r / r;
            kProd.g = rhs.g / g;
            kProd.b = rhs.b / b;
            kProd.a = rhs.a / a;

            return kProd;
        }

        inline Color operator / (const float fScalar ) const
        {
            assert( fScalar != 0.0 );

            Color kDiv;

            float fInv = 1.0f / fScalar;
            kDiv.r = r * fInv;
            kDiv.g = g * fInv;
            kDiv.b = b * fInv;
            kDiv.a = a * fInv;

            return kDiv;
        }

        inline friend Color operator * (const float fScalar, const Color& rkVector )
        {
            Color kProd;

            kProd.r = fScalar * rkVector.r;
            kProd.g = fScalar * rkVector.g;
            kProd.b = fScalar * rkVector.b;
            kProd.a = fScalar * rkVector.a;

            return kProd;
        }

        // arithmetic updates
        inline Color& operator += ( const Color& rkVector )
        {
            r += rkVector.r;
            g += rkVector.g;
            b += rkVector.b;
            a += rkVector.a;

            return *this;
        }

        inline Color& operator -= ( const Color& rkVector )
        {
            r -= rkVector.r;
            g -= rkVector.g;
            b -= rkVector.b;
            a -= rkVector.a;

            return *this;
        }

        inline Color& operator *= (const float fScalar )
        {
            r *= fScalar;
            g *= fScalar;
            b *= fScalar;
            a *= fScalar;
            return *this;
        }

        inline Color& operator /= (const float fScalar )
        {
            assert( fScalar != 0.0 );

            float fInv = 1.0f / fScalar;

            r *= fInv;
            g *= fInv;
            b *= fInv;
            a *= fInv;

            return *this;
        }

		/** Set a colour value from Hue, Saturation and Brightness.
		@param hue Hue value, scaled to the [0,1] range as opposed to the 0-360
		@param saturation Saturation level, [0,1]
		@param brightness Brightness level, [0,1]
		*/
		void setHSB(float hue, float saturation, float brightness);

		/** Convert the current colour to Hue, Saturation and Brightness values. 
		@param hue Output hue value, scaled to the [0,1] range as opposed to the 0-360
		@param saturation Output saturation level, [0,1]
		@param brightness Output brightness level, [0,1]
		*/
		void getHSB(float* hue, float* saturation, float* brightness) const;



		/** Function for writing to a stream.
		*/
		inline _DllExport friend std::ostream& operator <<
			( std::ostream& o, const Color& c )
		{
			o << "Color(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
			return o;
		}

    };
}

#endif