#ifndef _TITAN_FILESYSTEM__HH
#define  _TITAN_FILESYSTEM__HH

#include "TitanPrerequisites.h"
#include "TitanCommon.h"

namespace Titan
{
	//abstract class
	class _DllExport FileSystem : public GeneralAlloc
	{
	public:
		FileSystem(const String& name, const String& type): mName(name), mType(type){};

		virtual ~FileSystem(){};

		const String& getName() const{ return mName; }

		const String& getType() const { return mType; }

		virtual void load() = 0;

		virtual void unload() = 0;

		virtual StringVectorPtr		find(const String& wildcard, bool recursive) = 0;

		virtual void	find(const String& wildcard, bool recursive, StringVectorPtr* nameArray, StringVectorPtr* relPathArray) = 0;

	protected:
		String	mType;
		String	mName;
	};

	class _DllExport FileSystemFactory : public GeneralAlloc
	{
	public:
		FileSystemFactory(const String& type)
			:mType(type)
		{}

		virtual ~FileSystemFactory(){};

		const String&	getType() const { return mType; }

		virtual FileSystem*		createInstance(const String& name, const String& type) = 0;

		virtual void			destroyInstance(FileSystem* file) = 0;

	protected:
		String mType;
	};
}

#endif