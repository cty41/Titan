#ifndef _TITAN_REFLECTOR__HH
#define _TITAN_REFLECTOR__HH

#include "TiPrerequisites.h"
#include "TiException.h"
#include "TiCommon.h"

namespace Titan
{

	class _DllExport ParamsCommand: public GeneralAlloc
	{
	public:
		virtual ~ParamsCommand(){};

		virtual void setter(void* target, const String& val) = 0;

		virtual String getter(void* target) = 0;
	};

	class _DllExport ClassParamsDict
	{
	protected:
		typedef map<String, ParamsCommand*>::type ParamsCommandMap;
		ParamsCommandMap	mParamsCommandMap;

	public:
		ParamsCommand*	getParamsCommand(const String&  name)
		{
			ParamsCommandMap::iterator it = mParamsCommandMap.find(name);
			if(it != mParamsCommandMap.end())
			{
				return it->second;
			}
			else
			{
				return NULL;
			}
		}

		void  addParamsCommand(const String& name, ParamsCommand* command)
		{
			ParamsCommandMap::iterator it = mParamsCommandMap.find(name);
			if (it == mParamsCommandMap.end())
			{
				mParamsCommandMap[name] = command;
			}
			else
			{
				TITAN_EXCEPT_INTERNALERROR(
					"this paramCommand has existed:" + name);
			}
		}
	};
	typedef map<String, ClassParamsDict>::type ParamsDictMap;
	class _DllExport Reflector
	{
	protected:

		
		static ParamsDictMap		msParamsDictMap;

		String	mClassName;
		ClassParamsDict*	mClassParamsDict;
	public:
		Reflector(){}

		virtual ~Reflector(){}

		void setSingleParam(const String& name, const String& val)
		{
			ParamsCommand* command = mClassParamsDict->getParamsCommand(name);
			if (command)
			{
				command->setter(static_cast<void*>(this), val);
			}
			else
			{
				TITAN_EXCEPT_ITEMLOST(
					"we did not find this paramCommand named: " + name
				);
			}
		}

		void setParams(const StringKeyValueMap& paramMap)
		{
			StringKeyValueMap::const_iterator it = paramMap.begin(), itend = paramMap.end();
			for(;it != itend; ++it)
			{
				ParamsCommand* command = mClassParamsDict->getParamsCommand(it->first);
				if (command)
				{
					command->setter(static_cast<void*>(this), it->second);
				}
				else
				{
					TITAN_EXCEPT_ITEMLOST(
						"we did not find this paramCommand named: " + it->first
						);
				}
			}
		}

		bool createClassParamsDict(const String& className)
		{
			ParamsDictMap::iterator it = msParamsDictMap.find(className);
			if(it == msParamsDictMap.end())
			{
				mClassParamsDict = &msParamsDictMap.insert(ParamsDictMap::value_type(className, ClassParamsDict())).first->second;
				mClassName = className;

				return true;
			}
			else
			{
				mClassName = it->first;
				mClassParamsDict = &it->second;
				return false;
			}
		}

	protected:
		virtual void setupParamsCmd(){}


	};


}
#endif