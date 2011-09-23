#ifndef __TITANEXCEPTION__HH
#define __TITANEXCEPTION__HH
/************************************************************************/
/* copy from ogre														*/
/************************************************************************/
#include "TiPrerequisites.h"
#include <exception>

namespace Titan
{
	class _DllExport Exception: public std::exception
	{
	public:
		enum ExceptionTypes
		{
			EXCEP_RENDERAPI_ERROR,
			EXCEP_FILE_NOT_FOUND,
			EXCEP_ITEM_NOT_FOUND,
			EXCEP_CANNOT_WRITE_TO_FILE,
			EXCEP_INTERNAL_ERROR,
			EXCEP_INVALID_PARAMS, 
			EXCEP_INTERNAL_WARNNING,
		};

		Exception(int number, const String& desc, const String& src);

		Exception(int number, const String& desc, const String& src, const char* type, const char* file, long line);

		Exception(const Exception& rhs);

		~Exception() throw() {}

		void operator = (const Exception& rhs);

		virtual const String& getFullDescription() const;

		virtual int getNumber() const;

		virtual const String& getSource() const { return source; }

		virtual const String& getFile() const { return file; }

		virtual long getLine() const { return line; }

		virtual const String& getDescription() const { return description; }

		/// Override std::exception::what, check it -cty
		const char* what() throw() { return getFullDescription().c_str(); }

	protected:
		long line;
		int number;
		String typeName;
		String description;
		String source;
		String file;
		mutable String fullDesc;
	};

	/** Template struct which creates a distinct type for each exception code.
	@note
	This is useful because it allows us to create an overloaded method
	for returning different exception types by value without ambiguity. 
	From 'Modern C++ Design' (Alexandrescu 2001).
	*/
	// really nice skill - cty
	template <int num>
	struct ExceptionCodeType
	{
		enum { number = num };
	};
	//-------------------------------------------------------------------------------//
	class _DllExport FileNotFoundException: public Exception
	{
	public:
		FileNotFoundException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "FileNotFoundException", file, line){}

	};
	//-------------------------------------------------------------------------------//
	class _DllExport ItemNotFoundException: public Exception
	{
	public:
		ItemNotFoundException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "ItemNotFoundException", file, line){}

	};
	//-------------------------------------------------------------------------------//
	class _DllExport CanNotWriteToFileException: public Exception
	{
	public:
		CanNotWriteToFileException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "CanNotWriteToFileException", file, line){}

	};
	//-------------------------------------------------------------------------------//
	class _DllExport RenderAPIException: public Exception
	{
	public:
		RenderAPIException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "RenderAPIException", file, line){}
	};
	//-------------------------------------------------------------------------------//
	class _DllExport InternalErrorException: public Exception
	{
	public:
		InternalErrorException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "InternalErrorException", file, line){}
	};
	//-------------------------------------------------------------------------------//
	class _DllExport InvalidParamsException: public Exception
	{
	public:
		InvalidParamsException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "InvalidParamsException", file, line){}
	};
	//-------------------------------------------------------------------------------//
	class _DllExport InternalWarnningsException: public Exception
	{
	public:
		InternalWarnningsException(int number, const String& description, const String& source, const char* file, long line)
			: Exception(number, description,source, "InternalWarnningsException", file, line){}
	};

	
	class ExceptionFactory
	{
	private:
		ExceptionFactory();
	public:
		static FileNotFoundException create(
			ExceptionCodeType<Exception::EXCEP_FILE_NOT_FOUND> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return FileNotFoundException(code.number, desc, src, file, line);
		}
		//-------------------------------------------------------------------------------//
		static ItemNotFoundException create(
			ExceptionCodeType<Exception::EXCEP_ITEM_NOT_FOUND> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return ItemNotFoundException(code.number, desc, src, file, line);
		}

		static CanNotWriteToFileException create(
			ExceptionCodeType<Exception::EXCEP_CANNOT_WRITE_TO_FILE> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return CanNotWriteToFileException(code.number, desc, src, file, line);
		}

		static RenderAPIException create(
			ExceptionCodeType<Exception::EXCEP_RENDERAPI_ERROR> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return RenderAPIException(code.number, desc, src, file, line);
		}

		static InternalErrorException create(
			ExceptionCodeType<Exception::EXCEP_INTERNAL_ERROR> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return InternalErrorException(code.number, desc, src, file, line);
		}

		static InvalidParamsException create(
			ExceptionCodeType<Exception::EXCEP_INVALID_PARAMS> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return InvalidParamsException(code.number, desc, src, file, line);
		}

		static InternalWarnningsException create(
			ExceptionCodeType<Exception::EXCEP_INTERNAL_WARNNING> code,
			const String& desc,
			const String& src, const char* file, long line)
		{
			return InternalWarnningsException(code.number, desc, src, file, line);
		}
	};

#ifndef TITAN_EXCEPT
#define TITAN_EXCEPT(num, desc) throw Titan::ExceptionFactory::create(\
	Titan::ExceptionCodeType<num>(), desc, __FUNCTION__, __FILE__, __LINE__);

#define TITAN_EXCEPT_API(desc)				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, desc)
#define TITAN_EXCEPT_FILELOST(desc)			TITAN_EXCEPT(Exception::EXCEP_FILE_NOT_FOUND, desc)
#define TITAN_EXCEPT_ITEMLOST(desc)			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND, desc)
#define TITAN_EXCEPT_FAILWRITEFILE(desc)	TITAN_EXCEPT(Exception::EXCEP_CANNOT_WRITE_TO_FILE, desc)
#define TITAN_EXCEPT_INTERNALERROR(desc)	TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR, desc)
#define TITAN_EXCEPT_INVALIDPARAMS(desc)	TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, desc)
#define TITAN_EXCEPT_WARN(desc)				TITAN_EXCEPT(Exception::EXCEP_INTERNAL_WARNNING, desc)
#endif
}
#endif