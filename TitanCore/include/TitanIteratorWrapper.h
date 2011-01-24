#ifndef _TITAN_ITERATOR_WRAPPER_HH
#define _TITAN_ITERATOR_WRAPPER_HH
#include "TitanPrerequisites.h"

namespace Titan
{
	template <typename T, typename IteratorType, typename ValType>
	class IteratorWrapper :public GeneralAlloc
	{
	private:
		IteratorWrapper();
	protected:
		IteratorType mBegin;
		IteratorType mEnd;
		IteratorType mCurrent;

	public:
		typedef ValType	 ValueType;
		typedef ValType* PointerType;

		typedef IteratorType iterator;

		typedef IteratorType const_iterator;

		IteratorWrapper(IteratorType begin, IteratorType end)
		: mBegin(begin), mCurrent(begin), mEnd(end)
		{
		}
		//-------------------------------------------------------------//
		bool hasMoreElements() const
		{
			return mCurrent != mEnd;
		}
		//-------------------------------------------------------------//
		void next()
		{
			++mCurrent;
		}
		//-------------------------------------------------------------//
		const IteratorType& begin(){ return mBegin; }

		const IteratorType& current() { return mCurrent; }

		const IteratorType& end() { return mEnd; } 

	};

	template<typename T, typename IteratorType>
	class VectorIteratorWrapper: public IteratorWrapper<T, IteratorType, typename T::value_type>
	{
	public:
		typedef typename IteratorWrapper<T, IteratorType, typename T::value_type>::ValueType ValueType;
		typedef typename IteratorWrapper<T, IteratorType, typename T::value_type>::PointerType PointerType;

		VectorIteratorWrapper(IteratorType begin, IteratorType end)
		: IteratorWrapper<T, IteratorType, typename T::value_type>(begin, end)
		{
		}
		//-------------------------------------------------------------//
		/** Returns the next(=current) element in the collection, without advancing to the next. */
		ValueType peekNext (  ) const
		{
			return *(this->mCurrent);
		}
		
		/** Returns a pointer to the next(=current) element in the collection, without advancing to the next afterwards. */
		PointerType peekNextPtr (  )  const
		{
			return &(*(this->mCurrent) );
		}

		/** Returns the next(=current) value element in the collection, and advances to the next. */
		ValueType getNext (  ) 
		{
			return *(this->mCurrent++);
		}
	};

	template<typename T>
	class VectorIterator: public VectorIteratorWrapper<T, typename T::iterator>
	{
		VectorIterator( typename T::iterator begin, typename T::iterator end )
		: VectorIteratorWrapper<T,  typename T::iterator>(begin , end )
		{
		}

		explicit VectorIterator( T& c )
		: VectorIteratorWrapper<T,  typename T::iterator> ( c.begin(), c.end() )
		{
		}
	};

	template <typename T>
class ConstVectorIterator : public VectorIteratorWrapper<T,  typename T::const_iterator>
{
	public:
		ConstVectorIterator( typename T::const_iterator start, typename T::const_iterator end )
		: VectorIteratorWrapper<T,  typename T::const_iterator> (start , end )
		{
		}

		explicit ConstVectorIterator ( const T& c )
		 : VectorIteratorWrapper<T,  typename T::const_iterator> (c.begin() , c.end() )
		{
		}
};


template<typename T , typename IteratorType>
class MapIteratorWrapper: public IteratorWrapper<T, IteratorType, typename T::mapped_type>
{
public:
	typedef typename IteratorWrapper<T, IteratorType, typename  T::mapped_type>::ValueType ValueType;

	typedef typename IteratorWrapper<T, IteratorType, typename  T::mapped_type>::PointerType PointerType ;

	typedef typename T::value_type PairType ;

	typedef typename T::key_type KeyType;

	MapIteratorWrapper ( IteratorType begin, IteratorType end )
		: IteratorWrapper<T, IteratorType, typename T::mapped_type>( begin, end ) 
	{
	}
	//-------------------------------------------------------------//
	KeyType peekNextKey(void) const
	{
		return this->mCurrent->first;
	}

	ValueType peekNextValue (  ) const
	{
		return this->mCurrent->second;
	}
	//-------------------------------------------------------------//
	const PointerType peekNextValuePtr (  )  const
	{
		return &(this->mCurrent->second);
	}

	ValueType getNext()
	{
		return ((this->mCurrent++)->second) ;
	}	

};

template <typename T>
class MapIterator : public MapIteratorWrapper<T,  typename T::iterator>{
	public:

		MapIterator( typename T::iterator start, typename T::iterator end )
		: MapIteratorWrapper<T,  typename T::iterator>(start , end )
		{
		}

		explicit MapIterator( T& c )
		: MapIteratorWrapper<T,  typename T::iterator> ( c.begin(), c.end() )
		{
		}
		
};

template <typename T>
class ConstMapIterator : public MapIteratorWrapper<T,  typename T::const_iterator>{
	public:
	
		ConstMapIterator( typename T::const_iterator start, typename T::const_iterator end )
		: MapIteratorWrapper<T,  typename T::const_iterator> (start , end )
		{
		}

		explicit ConstMapIterator ( const T& c )
		 : MapIteratorWrapper<T,  typename T::const_iterator> (c.begin() , c.end() )
		{
		}
};

}
#endif