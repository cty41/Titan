//completely copied from ogre
#ifndef __TITAN_DATASTREAM_H__
#define __TITAN_DATASTREAM_H__

#include "TitanPrerequisites.h"
#include "TitanStringFuncs.h"
#include "TitanSharedPtr.h"
#include <istream>

namespace Titan {

	class _DllExport DataStream : public GeneralAlloc
	{
	public:
		enum AccessMode
		{
			READ = 1, 
			WRITE = 2
		};
	protected:
		/// The name (e.g. resource name) that can be used to identify the source fot his data (optional)
		String mName;		
        /// Size of the data in the stream (may be 0 if size cannot be determined)
        size_t mSize;
		/// What type of access is allowed (AccessMode)
		uint16 mAccess;

        #define STREAM_TEMP_SIZE 128
	public:
		/// Constructor for creating unnamed streams
        DataStream(uint16 accessMode = READ) : mSize(0), mAccess(accessMode) {}
		/// Constructor for creating named streams
		DataStream(const String& name, uint16 accessMode = READ) 
			: mName(name), mSize(0), mAccess(accessMode) {}
		/// Returns the name of the stream, if it has one.
		const String& getName(void) { return mName; }
		/// Gets the access mode of the stream
		uint16 getAccessMode() const { return mAccess; }
		/** Reports whether this stream is readable. */
		virtual bool isReadable() const { return (mAccess & READ) != 0; }
		/** Reports whether this stream is writeable. */
		virtual bool isWriteable() const { return (mAccess & WRITE) != 0; }
        virtual ~DataStream() {}
		// Streaming operators
        template<typename T> DataStream& operator>>(T& val);

		virtual size_t read(void* buf, size_t count) = 0;

		virtual size_t write(const void* buf, size_t count)
		{
                        (void)buf;
                        (void)count;
			// default to not supported
			return 0;
		}

		virtual size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
		
	    virtual String getLine( bool trimAfter = true );

	    virtual String getAsString(void);

		virtual size_t skipLine(const String& delim = "\n");

		/** Skip a defined number of bytes. This can also be a negative value, in which case
		the file pointer rewinds a defined number of bytes. */
		virtual void skip(long count) = 0;
	
		/** Repositions the read point to a specified byte.
	    */
	    virtual void seek( size_t pos ) = 0;
		
		/** Returns the current byte offset from beginning */
	    virtual size_t tell(void) const = 0;

		/** Returns true if the stream has reached the end.
	    */
	    virtual bool eof(void) const = 0;

		/** Returns the total size of the data to be read from the stream, 
			or 0 if this is indeterminate for this stream. 
		*/
        size_t size(void) const { return mSize; }

        /** Close the stream; this makes further operations invalid. */
        virtual void close(void) = 0;
		

	};

	/** Shared pointer to allow data streams to be passed around without
		worrying about deallocation
	*/
	typedef SharedPtr<DataStream> DataStreamPtr;

	/// List of DataStream items
	typedef list<DataStreamPtr>::type DataStreamList;
	/// Shared pointer to list of DataStream items
	typedef SharedPtr<DataStreamList> DataStreamListPtr;

	/** Common subclass of DataStream for handling data from chunks of memory.
	*/
	class _DllExport MemoryDataStream : public DataStream
	{
	protected:
        /// Pointer to the start of the data area
	    uchar* mData;
        /// Pointer to the current position in the memory
	    uchar* mPos;
        /// Pointer to the end of the memory
	    uchar* mEnd;
        /// Do we delete the memory on close
		bool mFreeOnClose;			
	public:

		MemoryDataStream(void* pMem, size_t size, bool freeOnClose = false, bool readOnly = false);

		MemoryDataStream(const String& name, void* pMem, size_t size, 
				bool freeOnClose = false, bool readOnly = false);

		MemoryDataStream(DataStream& sourceStream, 
				bool freeOnClose = true, bool readOnly = false);
		
		MemoryDataStream(DataStreamPtr& sourceStream, 
				bool freeOnClose = true, bool readOnly = false);


		MemoryDataStream(const String& name, DataStream& sourceStream, 
				bool freeOnClose = true, bool readOnly = false);


        MemoryDataStream(const String& name, const DataStreamPtr& sourceStream, 
            bool freeOnClose = true, bool readOnly = false);

		MemoryDataStream(size_t size, bool freeOnClose = true, bool readOnly = false);

		MemoryDataStream(const String& name, size_t size, 
				bool freeOnClose = true, bool readOnly = false);

		~MemoryDataStream();

		/** Get a pointer to the start of the memory block this stream holds. */
		uchar* getPtr(void) { return mData; }
		
		/** Get a pointer to the current position in the memory block this stream holds. */
		uchar* getCurrentPtr(void) { return mPos; }

		size_t read(void* buf, size_t count);

		size_t write(const void* buf, size_t count);


		size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");

		size_t skipLine(const String& delim = "\n");

		void skip(long count);

	    void seek( size_t pos );

	    size_t tell(void) const;

	    bool eof(void) const;

        void close(void);

		/** Sets whether or not to free the encapsulated memory on close. */
		void setFreeOnClose(bool free) { mFreeOnClose = free; }
	};

    /** Shared pointer to allow memory data streams to be passed around without
    worrying about deallocation
    */
    typedef SharedPtr<MemoryDataStream> MemoryDataStreamPtr;

    /** Common subclass of DataStream for handling data from 
		std::basic_istream.
	*/
	class _DllExport FileStreamDataStream : public DataStream
	{
	protected:
		/// Reference to source stream (read)
		std::istream* mpInStream;
		/// Reference to source file stream (read-only)
		std::ifstream* mpFStreamRO;
		/// Reference to source file stream (read-write)
		std::fstream* mpFStream;
        bool mFreeOnClose;	

		void determineAccess();
	public:
		FileStreamDataStream(std::ifstream* s, 
            bool freeOnClose = true);

		FileStreamDataStream(std::fstream* s, 
			bool freeOnClose = true);

		FileStreamDataStream(const String& name, 
            std::ifstream* s, 
            bool freeOnClose = true);

		FileStreamDataStream(const String& name, 
			std::fstream* s, 
			bool freeOnClose = true);

		FileStreamDataStream(const String& name, 
            std::ifstream* s, 
            size_t size, 
            bool freeOnClose = true);

		FileStreamDataStream(const String& name, 
			std::fstream* s, 
			size_t size, 
			bool freeOnClose = true);

		~FileStreamDataStream();

		size_t read(void* buf, size_t count);

		size_t write(const void* buf, size_t count);

        size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
		
		void skip(long count);

	    void seek( size_t pos );

		size_t tell(void) const;

	    bool eof(void) const;

        void close(void);
		
		
	};

}
#endif

