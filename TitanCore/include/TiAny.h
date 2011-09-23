// -- Based on boost::any, original copyright information follows --
// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompAnying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// -- End original copyright --

#ifndef __TITAN_ANY_H__
#define __TITAN_ANY_H__

#include "TiPrerequisites.h"
#include "TiException.h"
#include <algorithm>
#include <typeinfo>


namespace Titan
{
	class Any 
    {
    public: // constructors

        Any()
          : mContent(0)
        {
        }

        template<typename ValueType>
        explicit Any(const ValueType & value)
          : mContent(TITAN_NEW_T(holder<ValueType>, MEMCATEGORY_GENERAL)(value))
        {
        }

        Any(const Any & other)
          : mContent(other.mContent ? other.mContent->clone() : 0)
        {
        }

        virtual ~Any()
        {
            destroy();
        }

    public: // modifiers

        Any& swap(Any & rhs)
        {
            std::swap(mContent, rhs.mContent);
            return *this;
        }

        template<typename ValueType>
        Any& operator=(const ValueType & rhs)
        {
            Any(rhs).swap(*this);
            return *this;
        }

        Any & operator=(const Any & rhs)
        {
            Any(rhs).swap(*this);
            return *this;
        }

    public: // queries

        bool isEmpty() const
        {
            return !mContent;
        }

        const std::type_info& getType() const
        {
            return mContent ? mContent->getType() : typeid(void);
        }

		inline friend std::ostream& operator <<
			( std::ostream& o, const Any& v )
		{
			if (v.mContent)
				v.mContent->writeToStream(o);
			return o;
		}

		void destroy()
		{
			TITAN_DELETE_T(mContent, placeholder, MEMCATEGORY_GENERAL);
			mContent = NULL;
		}

    protected: // types

		class placeholder 
        {
        public: // structors
    
            virtual ~placeholder()
            {
            }

        public: // queries

            virtual const std::type_info& getType() const = 0;

            virtual placeholder * clone() const = 0;
    
			virtual void writeToStream(std::ostream& o) = 0;

        };

        template<typename ValueType>
        class holder : public placeholder
        {
        public: // structors

            holder(const ValueType & value)
              : held(value)
            {
            }

        public: // queries

            virtual const std::type_info & getType() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return TITAN_NEW_T(holder, MEMCATEGORY_GENERAL)(held);
            }

			virtual void writeToStream(std::ostream& o)
			{
				o << held;
			}


        public: // representation

            ValueType held;

        };



    protected: // representation
        placeholder * mContent;

        template<typename ValueType>
        friend ValueType * any_cast(Any *);


    public: 

	    template<typename ValueType>
    	ValueType operator()() const
    	{
			if (!mContent) 
			{
				TITAN_EXCEPT_INVALIDPARAMS(
					"Bad cast from uninitialised Any");
			}
			else if(getType() == typeid(ValueType))
			{
             	return static_cast<Any::holder<ValueType> *>(mContent)->held;
			}
			else
			{
				StringStream str;
				str << "Bad cast from type '" << getType().name() << "' "
					<< "to '" << typeid(ValueType).name() << "'";
				TITAN_EXCEPT_INVALIDPARAMS(
					 str.str());
			}
		}

		

    };


	/** Specialised Any class which has built in arithmetic operators, but can 
		hold only types which support operator +,-,* and / .
	*/
	class AnyNumeric : public Any
	{
	public:
		AnyNumeric()
		: Any()
		{
		}

		template<typename ValueType>
		AnyNumeric(const ValueType & value)
			
		{
			mContent = TITAN_NEW_T(numholder<ValueType>, MEMCATEGORY_GENERAL)(value);
		}

		AnyNumeric(const AnyNumeric & other)
            : Any()
		{
			mContent = other.mContent ? other.mContent->clone() : 0; 
		}

	protected:
		class numplaceholder : public Any::placeholder
		{
		public: // structors

			~numplaceholder()
			{
			}
			virtual placeholder* add(placeholder* rhs) = 0;
			virtual placeholder* subtract(placeholder* rhs) = 0;
			virtual placeholder* multiply(placeholder* rhs) = 0;
			virtual placeholder* multiply(float factor) = 0;
			virtual placeholder* divide(placeholder* rhs) = 0;
		};

		template<typename ValueType>
		class numholder : public numplaceholder
		{
		public: // structors

			numholder(const ValueType & value)
				: held(value)
			{
			}

		public: // queries

			virtual const std::type_info & getType() const
			{
				return typeid(ValueType);
			}

			virtual placeholder * clone() const
			{
				return TITAN_NEW_T(numholder, MEMCATEGORY_GENERAL)(held);
			}

			virtual placeholder* add(placeholder* rhs)
			{
				return TITAN_NEW_T(numholder, MEMCATEGORY_GENERAL)(held + static_cast<numholder*>(rhs)->held);
			}
			virtual placeholder* subtract(placeholder* rhs)
			{
				return TITAN_NEW_T(numholder, MEMCATEGORY_GENERAL)(held - static_cast<numholder*>(rhs)->held);
			}
			virtual placeholder* multiply(placeholder* rhs)
			{
				return TITAN_NEW_T(numholder, MEMCATEGORY_GENERAL)(held * static_cast<numholder*>(rhs)->held);
			}
			virtual placeholder* multiply(float factor)
			{
				return TITAN_NEW_T(numholder, MEMCATEGORY_GENERAL)(held * factor);
			}
			virtual placeholder* divide(placeholder* rhs)
			{
				return TITAN_NEW_T(numholder, MEMCATEGORY_GENERAL)(held / static_cast<numholder*>(rhs)->held);
			}
			virtual void writeToStream(std::ostream& o)
			{
				o << held;
			}

		public: // representation

			ValueType held;

		};

		/// Construct from holder
		AnyNumeric(placeholder* pholder)
		{
			mContent = pholder;
		}

	public:
		AnyNumeric & operator=(const AnyNumeric & rhs)
		{
			AnyNumeric(rhs).swap(*this);
			return *this;
		}
		AnyNumeric operator+(const AnyNumeric& rhs) const
		{
			return AnyNumeric(
				static_cast<numplaceholder*>(mContent)->add(rhs.mContent));
		}
		AnyNumeric operator-(const AnyNumeric& rhs) const
		{
			return AnyNumeric(
				static_cast<numplaceholder*>(mContent)->subtract(rhs.mContent));
		}
		AnyNumeric operator*(const AnyNumeric& rhs) const
		{
			return AnyNumeric(
				static_cast<numplaceholder*>(mContent)->multiply(rhs.mContent));
		}
		AnyNumeric operator*(float factor) const
		{
			return AnyNumeric(
				static_cast<numplaceholder*>(mContent)->multiply(factor));
		}
		AnyNumeric operator/(const AnyNumeric& rhs) const
		{
			return AnyNumeric(
				static_cast<numplaceholder*>(mContent)->divide(rhs.mContent));
		}
		AnyNumeric& operator+=(const AnyNumeric& rhs)
		{
			*this = AnyNumeric(
				static_cast<numplaceholder*>(mContent)->add(rhs.mContent));
			return *this;
		}
		AnyNumeric& operator-=(const AnyNumeric& rhs)
		{
			*this = AnyNumeric(
				static_cast<numplaceholder*>(mContent)->subtract(rhs.mContent));
			return *this;
		}
		AnyNumeric& operator*=(const AnyNumeric& rhs)
		{
			*this = AnyNumeric(
				static_cast<numplaceholder*>(mContent)->multiply(rhs.mContent));
			return *this;
		}
		AnyNumeric& operator/=(const AnyNumeric& rhs)
		{
			*this = AnyNumeric(
				static_cast<numplaceholder*>(mContent)->divide(rhs.mContent));
			return *this;
		}




	};


    template<typename ValueType>
    ValueType * any_cast(Any * operand)
    {
        return operand && operand->getType() == typeid(ValueType)
                    ? &static_cast<Any::holder<ValueType> *>(operand->mContent)->held
                    : 0;
    }

    template<typename ValueType>
    const ValueType * any_cast(const Any * operand)
    {
        return any_cast<ValueType>(const_cast<Any *>(operand));
    }

    template<typename ValueType>
    ValueType any_cast(const Any & operand)
    {
        const ValueType * result = any_cast<ValueType>(&operand);
        if(!result)
		{
			StringStream str;
			str << "Bad cast from type '" << operand.getType().name() << "' "
				<< "to '" << typeid(ValueType).name() << "'";
			TITAN_EXCEPT_INVALIDPARAMS(
				str.str());
		}
        return *result;
    }
	/** @} */
	/** @} */


}

#endif

