#ifndef _TITAN_RESOURCEGROUPMANAGER__HH
#define _TITAN_RESOURCEGROUPMANAGER__HH

#include "TitanPrerequisites.h"

namespace Titan
{
	class _DllExport ResourceGroupManager: public GeneralAlloc
	{
	public:
		struct ResourceGroup
		{
			enum Status
			{
				RGS_UNLOADED = 0,
				RGS_LOADING	 = 1,
				RGS_LOADED	 = 2
			};
			String	name;
			Status	state;
			typedef list<Resource*>::type ResourceList;
		};
		typedef map<String, ResourceGroup*>::type ResourceGroupMap;
	public:
		ResourceGroupManager();

		virtual ~ResourceGroupManager();



	protected:
	};
}
#endif