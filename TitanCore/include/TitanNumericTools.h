#ifndef __TITAN_NUMERICTOOLS__HH
#define __TITAN_NUMERICTOOLS__HH

#include "TiPrerequisites.h"

namespace Titan
{

	#define DEFAULT_EPSILON	((float)0.00001f)

// sign testing
template <class T> T absoluteValue(T value);
template <class T> bool isPositive(T a);
template <class T> bool isNegative(T a);
template <class T> bool sameSigns(T a, T b);
template <class T> T copySign(T value, T s);

// value testing
template <class T> bool deltaRangeTest(T a, T b, T epsilon);
template <class T> bool deltaRangeTest(T a, T b);
template <class T> const T& minimum(const T& a, const T& b);
template <class T> const T& maximum(const T& a, const T& b);

// value clamping
template <class T> T clamp(T value, T low, T high);
template <class T> T clampPositive(T value);
template <class T> T clampNegative(T value);
float clampUnitSize(float value);

// bit testing
template<class T> int highestBitSet(T input);
template<class T> int lowestBitSet (T input);

// power of two functions
template<class T> bool isPowerOfTwo(T input);
template<class T> T nearestPowerOfTwo(T input);
template<class T> T ceilingPowerOfTwo(T input);
template<class T> T floorPowerOfTwo(T input);

// simple calculations
template <class T> T raiseToPower(T value, T power);
template <class T> T modulus(T value, T Y);
template <class T> T align(const T& value, T alignment);

// floating-point specific inverse (1/value)
float inverse(float value);

// round floating point numbers to the desired precision
float trimFloat(float input, uint8 precision);

// handy enum set for trimFloat precision values
enum trimFloat_PrecisionValues
{
	tf_whole_unit = 0,	// 0 bits after binary point
	tf_half_unit,		// 1 bit
	tf_4th_unit,		// 2 bits
	tf_8th_unit,		// 3 bits
	tf_16th_unit,		// 4 bits
	tf_32nd_unit,		// 5 bits
	tf_64th_unit,		// 6 bits
	tf_128th_unit,		// 7 bits
	tf_256th_unit,		// 8 bits
};

// real to integer conversion methods
int32 realToInt32(float input);			// default rounding method (fastest)
int32 realToInt32_chop(float input);		// truncation method 
int32 realToInt32_floor(float input);		// floor method 
int32 realToInt32_ceiling(float input);	// ceiling method 

// identical methods which accept a double (and truncate it to 32bit)
int32 realToInt32(double input);			// default rounding method (fastest)
int32 realToInt32_chop(double input);		// truncation method 
int32 realToInt32_floor(double input);		// floor method 
int32 realToInt32_ceiling(double input);	// ceiling method 

template <class _OUT, class _IN> inline _OUT fast_cast(_IN input)
{
	return (static_cast<_OUT>(input)); 
}
template<> inline int32 fast_cast(float input)
{
	return realToInt32_chop(input); 
}
template<> inline int32 fast_cast(double input)
{
	return realToInt32_chop((float)input); 
}

/*	
-----------------------------------------------------------------
	
	Floating Point Macros
	
-----------------------------------------------------------------
*/
// reinterpret a float as an int32
#define fpBits(f) (*reinterpret_cast<const int32*>(&(f))) 

// reinterpret an int32 as a float
#define intBits(i) (*reinterpret_cast<const float*>(&(i))) 

// return 0 or -1 based on the sign of the float
#define fpSign(f) (fpBits(f)>>31) 

// extract the 8 bits of exponent as a signed integer
// by masking out this bits, shifting down by 23,
// and subtracting the bias value of 127
#define fpExponent(f) (((fpBits(f)&0x7fffffff)>>23)-127) 

// return 0 or -1 based on the sign of the exponent
#define fpExponentSign(f) (fpExponent(f)>>31) 

// get the 23 bits of mantissa without the implied bit
#define fpPureMantissa(f) ((fpBits(f)&0x7fffff)) 

// get the 23 bits of mantissa with the implied bit replaced
#define fpMantissa(f) (fpPureMantissa(f) | (1<<23)) 

#define fpOneBits 0x3F800000

// flipSign is a helper Macro to
// invert the sign of i if flip equals -1, 
// if flip equals 0, it does nothing
#define flipSign(i, flip) ((i^ flip) - flip)



//:	absoluteValue
//-------------------------------------------------------------------------------//-------------------
//
//	Returns the absolute value of the input parameter
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T absoluteValue(T value)
{
	return abs(value); // call the standard C abs()
}

// handle floating point values
template <> 
inline float absoluteValue(float value)
{
	uint32 absValue = fpBits(value);
	absValue &= 0x7fffffff;

	return intBits(absValue);
}

// yes, we even handle unsigned values (quickly)
template <> inline uint8 absoluteValue(uint8 a) {return a;}
template <> inline uint16 absoluteValue(uint16 a) {return a;}
template <> inline uint32 absoluteValue(uint32 a) {return a;}



//:	isPositive
//-------------------------------------------------------------------------------//-------------------
//
//	Returns true if the provided value is positive
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline bool isPositive(T a)
{
	return (a >= 0) ? true : false;
}

// floating point version
template <> 
inline bool isPositive(float a)
{
	return !fpSign(a);
}

// yes, we even handle unsigned values (quickly)
template <> inline bool isPositive(uint8 a) {return true;}
template <> inline bool isPositive(uint16 a) {return true;}
template <> inline bool isPositive(uint32 a) {return true;}



//:	isNegative
//-------------------------------------------------------------------------------//-------------------
//
//	Returns true if the provided value is negative
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline bool isNegative(T a)
{
	return (a < 0) ? true : false;
}

// floating point version
template <> 
inline bool isNegative(float a)
{
	return fpSign(a) ? true:false;
}

// yes, we even handle unsigned values (quickly)
template <> inline bool isNegative(uint8 a) {return false;}
template <> inline bool isNegative(uint16 a) {return false;}
template <> inline bool isNegative(uint32 a) {return false;}



//:	sameSigns
//-------------------------------------------------------------------------------//-------------------
//
//	Returns true if the two provided values have the same sign
//
//-------------------------------------------------------------------------------//----------------://
template<class T> 
inline bool sameSigns(T a, T b)
{
	return (isNegative(a) == isNegative(b));
}



//:	copySigns
//-------------------------------------------------------------------------------//-------------------
//
//	Returns value with the sign of s
//
//-------------------------------------------------------------------------------//----------------://
template<class T> 
inline T copySign(T value, T s)
{
	return isNegative(s) ? -absoluteValue(value) : absoluteValue(value);
}



//:	deltaRangeTest
//-------------------------------------------------------------------------------//-------------------
//
//	Returns true is the delta between a and b is less than epsilon
//
//-------------------------------------------------------------------------------//----------------://
template <class T>
inline bool deltaRangeTest(T a, T b, T epsilon)
{
	return (absoluteValue(a - b) < epsilon) ? true : false;
}

//:	deltaRangeTest (default epsilon)
//-------------------------------------------------------------------------------//-------------------
//
//	Returns true is the delta between a and b is less than DEFAULT_EPSILON
//
//-------------------------------------------------------------------------------//----------------://
template <class T>
inline bool deltaRangeTest(T a, T b)
{
	return (a==b);
}
template <>
inline bool deltaRangeTest(float a, float b)
{
	return (absoluteValue(a - b) < DEFAULT_EPSILON) ? true : false;
}

//:	min
//-------------------------------------------------------------------------------//-------------------
//
//	Returns the lesser value of a abd b
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline const T& minimum(const T& a, const T& b)
{
	return a < b ? a : b;
}



//:	max
//-------------------------------------------------------------------------------//-------------------
//
//	Returns the greater value of a abd b
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline const T& maximum(const T& a, const T& b)
{
	return a > b ? a : b;
}



//:	clamp
//-------------------------------------------------------------------------------//-------------------
//
//	Returns value clamped between low and high so that (low <= value <= high)
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T clamp(T value, T low, T high)
{
	if(value < low)
	{
		return low;
	}
	
	if(value > high)
	{
		return high;
	} 
		
	return value;
}



//:	clampPositive
//-------------------------------------------------------------------------------//-------------------
//
//	Returns value so that (value >= 0)
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T clampPositive(T value)
{
	return value < 0 ? 0 : value;
}

template <> 
inline float clampPositive(float input)
{
	// if the value is negative, set it to zero
	int value = fpBits(input);
	int sign_mask = ~fpSign(input);
	value &= sign_mask;

	return intBits(value);
}



//:	clampNegative
//-------------------------------------------------------------------------------//-------------------
//
//	Returns value so that (value <= 0)
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T clampNegative(T value)
{
	return value > 0 ? 0 : value;
}

template <> 
inline float clampNegative(float input)
{
	// if the value is positive, set it to zero
	int value = fpBits(input);
	int sign_mask = fpSign(input);
	value &= sign_mask;
	return intBits(value);
}



//:	clampUnitSize
//-------------------------------------------------------------------------------//-------------------
//
//	For real data types only. Returns value so that (-1.0 < value < 1.0)
//
//-------------------------------------------------------------------------------//----------------://
inline float clampUnitSize(float input)
{
	// if the absolute value is greater than one, 
	// set it to one
	uint32 value = fpBits(input);
	uint32 abs_value = value & 0x7fffffff;
	abs_value -= (127<<23);
	abs_value >>= 31;
	uint32 one = (127<<23) & ~abs_value;
	value = (value & abs_value) + one;
	return intBits(value);
} 



//:	highestBitSet
//-------------------------------------------------------------------------------//-------------------
//
//	Returns the index of the highest bit set in the input value.
//
//-------------------------------------------------------------------------------//----------------://
template<class T>
inline int highestBitSet(T input)
{
	register int result;
	assert(input); // zero is invalid input!
	assert(sizeof(T)==4); // 32bit data only!
	_asm bsr eax, input
	_asm mov result, eax
	return result;
}

template<>
inline int highestBitSet (uint8 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dl, input // copy into a 32bit reg
	_asm and edx, 0xff // keep only the bits we want
	_asm bsr eax, edx // perform the scan
	_asm mov result, eax
	return result;
}
template<>
inline int highestBitSet (int8 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dl, input // copy into a 32bit reg
	_asm and edx, 0xff // keep only the bits we want
	_asm bsr eax, edx // perform the scan
	_asm mov result, eax
	return result;
}

template<>
inline int highestBitSet (uint16 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dx, input // copy into a 32bit reg
	_asm and edx, 0xffff // keep only the bits we want
	_asm bsr eax, edx // perform the scan
	_asm mov result, eax
	return result;
}
template<>
inline int highestBitSet (int16 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dx, input // copy into a 32bit reg
	_asm and edx, 0xffff // keep only the bits we want
	_asm bsr eax, edx // perform the scan
	_asm mov result, eax
	return result;
}

template<>
inline int highestBitSet (float f)
{
	register int result;
	register uint32 input = fpBits(f);
	assert(input); // zero is invalid input!
	_asm bsr eax, input
	_asm mov result, eax
	return result;
}



//:	lowestBitSet
//-------------------------------------------------------------------------------//-------------------
//
//	Returns the index of the lowest bit set in the input value.
//
//-------------------------------------------------------------------------------//----------------://
template<class T>
inline int lowestBitSet(T input)
{
	register int result;
	assert(input); // zero is invalid input!
	assert(sizeof(T)==4); // 32bit data only!
	_asm bsf eax, input
	_asm mov result, eax
	return result;
}

template<>
inline int lowestBitSet (uint8 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dl, input // copy into a 32bit reg
	_asm and edx, 0xff // keep only the bits we want
	_asm bsf eax, edx // perform the scan
	_asm mov result, eax
	return result;
}
template<>
inline int lowestBitSet (int8 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dl, input // copy into a 32bit reg
	_asm and edx, 0xff // keep only the bits we want
	_asm bsf eax, edx // perform the scan
	_asm mov result, eax
	return result;
}

template<>
inline int lowestBitSet (uint16 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dx, input // copy into a 32bit reg
	_asm and edx, 0xffff // keep only the bits we want
	_asm bsf eax, edx // perform the scan
	_asm mov result, eax
	return result;
}
template<>
inline int lowestBitSet (int16 input)
{
	register int result;
	assert(input); // zero is invalid input!
	_asm mov dx, input // copy into a 32bit reg
	_asm and edx, 0xffff // keep only the bits we want
	_asm bsf eax, edx // perform the scan
	_asm mov result, eax
	return result;
}

template<>
inline int lowestBitSet (float f)
{
	register int result;
	register uint32 input = fpBits(f);
	assert(input); // zero is invalid input!
	_asm bsf eax, input
	_asm mov result, eax
	return result;
}



//:	isPowerOfTwo
//-------------------------------------------------------------------------------//-------------------
//
//	Returns true if the input value is a power-of-teo (1,2,4,8,16, etc..)
//
//-------------------------------------------------------------------------------//----------------://
template<class T>
inline bool isPowerOfTwo(T input)
{
	// if the value is greater than zero, and has only one
	// bit set, it must be a power of two
	return (input>0 && highestBitSet(input) == lowestBitSet(input));
}

template<>
inline bool isPowerOfTwo(float input)
{
	// for floating-point values, we know the value is a power-of-two if
	// the mantissa is zero (not including the implied bit)
	return (!fpPureMantissa(input));
}



//:	nearestPowerOfTwo
//-------------------------------------------------------------------------------//-------------------
//
//	Rounds the input value to the nearest power-of-two. 
//  All values below 1 generate a result of 1
//
//-------------------------------------------------------------------------------//----------------://
template<class T>
inline T nearestPowerOfTwo(T input)
{
	
	// the least possible power-of-two value is 1
	if (input <= 1) return 1;

	int highestBit = highestBitSet(input);
	int roundingTest = input & (1<< (highestBit-1)); 
	if (roundingTest) ++highestBit;
	return static_cast<T>(1<<highestBit);
}

template<>
inline float nearestPowerOfTwo(float input)
{
	// convert the value to an int
	int result = fpBits(input);

	// if the value is negative, or less than 1.0f, return 1.0f
	// this mask test for the sign bit and the exponents sign in one step
	if (result & 0xc0000000) return 1.0f;

	// if anything is in the high bit of the mantissa, 
	// use it to add one to the exponent
	result += (result & 0x800000)<<1;

	// trim away the mantissa
	result &= ~((1<<23)-1);

	// convert back to floating-point as we return
	return (intBits(result));
}



//:	ceilingPowerOfTwo
//-------------------------------------------------------------------------------//-------------------
//
//	Rounds the next-highest power-of-two value. 
//  All values below 1 generate a result of 1
//
//-------------------------------------------------------------------------------//----------------://
template<class T>
inline T ceilingPowerOfTwo(T input)
{
	// the least possible power-of-two value is 1
	if (input <= (T)1) return 1;

	int highestBit = highestBitSet(index);
	int mask = input & ((1<< highestBit)-1); 
	highestBit += mask & 1;
	return static_cast<T>(1<<highestBit);
}

template<>
inline float ceilingPowerOfTwo(float input)
{
	// convert the value to an int
	int result = fpBits(input);

	// if the value is negative, or less than 1.0f, return 1.0f
	// this mask test for the sign bit and the exponents sign in one step
	if (result & 0xc0000000) return 1.0f;

	// if anything is in the mantissa, round up
	result += 0x7fffff;

	// trim away the mantissa
	result &= ~((1<<23)-1);

	// convert back to floating-point as we return
	return (intBits(result));
}



//:	floorPowerOfTwo
//-------------------------------------------------------------------------------//-------------------
//
//	Rounds the next-least power-of-two value. 
//  All values below 1 generate a result of 1
//
//-------------------------------------------------------------------------------//----------------://
template<class T>
inline T floorPowerOfTwo(T input)
{
	// the least possible power-of-two value is 1
	if (input <= (T)1) return 1;

	int highestBit = highestBitSet(input);
	return static_cast<T>(1<<highestBit);
}

template<>
inline float floorPowerOfTwo(float input)
{
	// convert the value to an int
	int result = fpBits(input);

	// if the value is negative, or less than 1.0f, return 1.0f
	// this mask test for the sign bit and the exponents sign in one step
	if (result & 0xc0000000) return 1.0f;

	// trim away the mantissa
	result &= ~((1<<23)-1);

	// convert back to floating-point as we return
	return (intBits(result));
}



//:	raiseToPower
//-------------------------------------------------------------------------------//-------------------
//
//	Calculates the value of a given Number raised to Power.
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T raiseToPower(T number, T power)
{
	return (value^power); 
}

template <> 
inline float raiseToPower(float number, float power)
{
	return (float)(pow(number, power)); 
}



//:	modulus
//-------------------------------------------------------------------------------//-------------------
//
//	Rounds the next-least power-of-two value. 
//  All values below 1 generate a result of 1
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T modulus(T a, T b)
{
	return (a%b); 
}

template <> 
inline float modulus(float a, float b)
{
	return (float)(fmod(a, b)); 
}



//:	alignUp
//-------------------------------------------------------------------------------//-------------------
//
//	returns the input value, aligned to the next higher multiple of alignment
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T alignUp(const T& value, T alignment)
{
	T remainder = modulus(value, alignment);

	if (remainder == 0)
	{
		return(Value);
	}

	return(value + (alignment - remainder));
}



//:	alignDown
//-------------------------------------------------------------------------------//-------------------
//
//	returns the input value, aligned to the next lower multiple of alignment
//
//-------------------------------------------------------------------------------//----------------://
template <class T> 
inline T alignDown(const T& value, T alignment)
{
	T remainder = modulus(value, alignment);

	if (remainder == 0)
	{
		return(Value);
	}

	return(value - remainder);
}






//:	inverse
//-------------------------------------------------------------------------------//-------------------
//
//	Calculates the inverse of a given value. (1/value)
//
//-------------------------------------------------------------------------------//----------------://
inline float inverse(float value)
{
	int _i = 2 * fpOneBits - fpBits(value);                                 
	float r = intBits(_i);                                                       
	return fpBits(value) ? (r * (2.0f - (value) * r)) : 0;  
}


//:	trimFloat
//-------------------------------------------------------------------------------//-------------------
//
//	Rounds the input value to the desired precision. Precision is stated as the number
//	of bits used to store the fractional value. Fractional bits are negative exponents,
//	1 bit equals a precision of half a unit (0.5f), 2 bits equals one quarter (0.25f), etc.
//
//-------------------------------------------------------------------------------//----------------://
inline float trimFloat(float input, uint8 precision)
{
	float result = input;
	int32 exponent      = fpExponent(input);
	int32 bias          = 23 - (exponent + precision);

	if (bias < 1)
	{
		return result;
	}
	else if (bias > 24)
	{
		return 0.0f;
	}
	else if (bias == 24)
	{
		int32 value = fpBits(input);
		value &= (1<<31);
		exponent = -precision;
		value += (exponent+127)<<23;
		return intBits(value);
	}

	int32 value         = fpBits(input);
	_asm
	{
		clc
		mov ecx, bias
		mov eax, value

		shr eax, cl
		adc eax, 0
		shl eax, cl

		mov value, eax
	};

	return intBits(value);
};



//:	realToInt32
//-------------------------------------------------------------------------------//-------------------
//
//	Convert a float value to int32, using the default rounding method set on the FPU.
//
//-------------------------------------------------------------------------------//----------------://
inline int32 realToInt32(float input)
{
	int32 result;

	__asm fld   input
	__asm fistp DWORD PTR result

	return result;
}



//:	realToInt32_chop
//-------------------------------------------------------------------------------//-------------------
//
//	Convert a float value to int32, all fractional values are truncated.
//	realToInt32_chop(2.35) = 2; realToInt32_chop(-2.35) = -2
//
//-------------------------------------------------------------------------------//----------------://
inline int32 realToInt32_chop(float input)
{
	// read the exponent and decide how much we need to shift the mantissa down
	int32 shift = 23-fpExponent(input);
	// read the mantissa and shift it down to remove all fractional values
	int32 result = fpMantissa(input)>>shift;
	// set the sign of the new result
	result = flipSign(result, fpSign(input));
	// if the exponent was negative, (-1<input<1) we must return zero
	result &= ~fpExponentSign(input);
	// return the result
	return result;                  
}


//:	realToInt32_floor
//-------------------------------------------------------------------------------//-------------------
//
//	Convert a float value to the next-lowest int32 value.
//	realToInt32_chop(2.35) = 2; realToInt32_chop(-2.35) = -3
//
//-------------------------------------------------------------------------------//----------------://
inline int32 realToInt32_floor(float input)
{ 
	// read the exponent and decide how much we need to shift the mantissa down
	int shift = 23-fpExponent(input);
	// read the mantissa and shift it down to remove all fractional values
	int result = fpMantissa(input)>>shift;
	// set the sign of the new result
	result = flipSign(result, fpSign(input));
	// if the exponent was negative, (-1<input<1) we must return zero
	result &= ~fpExponentSign(input);

	// if the original value is negative, and any fractional values are present,
	// decrement the result by one
	result -= fpSign(input) && (fpExponentSign(input) || (fpPureMantissa(input) & ((1<<shift)-1)));

	// return the result
	return result;                  
}



//:	realToInt32_ceiling
//-------------------------------------------------------------------------------//-------------------
//
//	Convert a float value to the next-highest int32 value.
//	realToInt32_chop(2.35) = 3; realToInt32_chop(-2.35) = -2
//
//-------------------------------------------------------------------------------//----------------://
inline int32 realToInt32_ceiling(float input)
{ 
	// read the exponent and decide how much we need to shift the mantissa down
	int shift = 23-fpExponent(input);
	// read the mantissa and shift it down to remove all fractional values
	int result = fpMantissa(input)>>shift;
	// set the sign of the new result
	result = flipSign(result, fpSign(input));
	// if the exponent was negative, (-1<input<1) we must return zero
	result &= ~fpExponentSign(input);

	// if the original value is positive and not zero, and any fractional values are present,
	// increment the result by one
	result += (!fpSign(input) && fpBits(input)) && (fpExponentSign(input) || (fpPureMantissa(input) & ((1<<shift)-1)));

	// return the result
	return result;                  
}


inline int32 realToInt32(double input)
{
	return realToInt32((float)input);
}

inline int32 realToInt32_chop(double input)
{
	return realToInt32_chop((float)input);
}

inline int32 realToInt32_floor(double input)
{
	return realToInt32_floor((float)input);
}

inline int32 realToInt32_ceiling(double input)
{
	return realToInt32_ceiling((float)input);
}

}
#endif