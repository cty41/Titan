#ifndef __TITAN_BITFLAG__HH
#define __TITAN_BITFLAG__HH

#include "TitanPrerequisites.h"
#pragma warning( disable : 4800 )

namespace Titan
{

#define FLAG(x) (1<<(x))
#define TEST_BIT(flag, bit) (((flag.value) & FLAG((bit))) !=0)
#define TEST_ALL(flag, set) (((flag.value) & (set)) == (set))
#define TEST_ANY(flag, set) (((flag.value) & (set)) !=0)
#define SET_BIT(flag, bit) ((flag.value) |= FLAG((bit)))
#define CLEAR_BIT(flag, bit) ((flag.value) &= ~FLAG((bit)))
#define SET_ALL(flag, set) ((flag.value) |= (set))
#define CLEAR_ALL(flag, set) ((flag.value) &= ~(set))

	template <class T>
	class BitFlags
	{
	public:

		T value;

		// Creators...
		BitFlags();
		BitFlags(T data);
		BitFlags(const BitFlags& Src);
		~BitFlags();

		// Operators...
		BitFlags& operator=( const BitFlags& Src);
		BitFlags& operator=( T Src);
		operator T()const {return(value);};
		bool operator==(const BitFlags& Src)const;
		bool operator!=(const BitFlags& Src)const;

		// Mutators...
		void set(T settings);
		void clear();
		void setFlags(T settings);
		void clearFlags(T settings);
		void setBit(int bit);
		void clearBit(int bit);
		void setBit(int bit, bool setting);

		// Accessors...
		bool isEmpty()const;
		bool testBit(int bit)const;
		bool testFlags(T test)const;
		bool testAny(T test)const;

		int	totalBits()const;
		int	totalSet()const;
	};

	// common flag typedefs
	typedef BitFlags<uint8> u8Flags;	// 8 bits of flags
	typedef BitFlags<uint16> u16Flags; // 16 bits of flags
	typedef BitFlags<uint32> u32Flags; // 32 bits of flags

	//- Inline Functions --------------------------------------------

	/*	BitFlags
	------------------------------------------------------------------------------------------

	Default Object Constructor

	------------------------------------------------------------------------------------------
	*/
	template <class T>
	inline BitFlags<T>::BitFlags()
		:value(0)
	{
	}

	template <class T>
	inline BitFlags<T>::BitFlags(T settings)
		:value(settings)
	{
	}

	template <class T>
	inline BitFlags<T>::BitFlags( const BitFlags& Src)
		:value(Src.value)
	{
	}

	/*	~BitFlags
	------------------------------------------------------------------------------------------

	Default Object Destructor

	------------------------------------------------------------------------------------------
	*/
	template <class T>
	inline BitFlags<T>::~BitFlags()
	{
	}

	//. Operators ............................................................................
	template <class T>
	inline BitFlags<T>& BitFlags<T>::operator=( const BitFlags<T>& Src)
	{
		value = Src.value;
		return(*this);
	}

	template <class T>
	inline BitFlags<T>& BitFlags<T>::operator=( T Src)
	{
		value = Src;
		return(*this);
	}

	template <class T>
	bool BitFlags<T>::operator==( const BitFlags& Src)const
	{
		return(value == Src.value);
	}

	template <class T>
	bool BitFlags<T>::operator!=( const BitFlags& Src)const
	{
		return(value != Src.value);
	}

	//. Mutators .............................................................................
	template <class T>
	inline void BitFlags<T>::set(T settings)
	{
		value = settings;
	}

	template <class T>
	inline void BitFlags<T>::clear()
	{
		value = 0;
	}

	template <class T>
	inline void BitFlags<T>::setFlags(T settings)
	{
		value |= settings;
	}

	template <class T>
	inline void BitFlags<T>::clearFlags(T settings)
	{
		value &= ~settings;
	}


	template <class T>
	inline void BitFlags<T>::setBit(int bit)
	{
		value |= (1<<bit);
	}

	template <class T>
	inline void BitFlags<T>::setBit(int bit, bool setting)
	{
		if (setting)
		{
			value |= (1<<bit);
		}
		else
		{
			clearBit(bit);
		}
	}

	template <class T>
	inline void BitFlags<T>::clearBit(int bit)
	{
		value &= (~(1<<bit));
	}

	//. Accessors ............................................................................
	template <class T>
	inline bool BitFlags<T>::isEmpty()const
	{
		return(value == static_cast<T>(0));
	}

	template <class T>
	inline bool BitFlags<T>::testBit(int bit)const
	{
		return(value & (1<<bit));
	}


	template <class T>
	inline bool BitFlags<T>::testFlags(T test)const
	{
		return((value & test) == test);
	}

	template <class T>
	inline bool BitFlags<T>::testAny(T test)const
	{
		return(value & test);
	}

	template <class T>
	inline int BitFlags<T>::totalBits()const
	{
		return(sizeof(T)<<3);
	}

	template <class T>
	inline int BitFlags<T>::totalSet()const
	{
		int count=0;
		int total = totalBits();
		T TestValue = value;

		for (int i=total;i;--i)
		{
			count += (TestValue & 1);
			TestValue >>= 1;
		}
		return(count);
	}

#pragma warning( default : 4800 )

}

#endif