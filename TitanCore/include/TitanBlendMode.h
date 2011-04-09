#ifndef  __TITAN_BLEND_MODE__HH
#define	 __TITAN_BLEND_MODE__HH


#include "TitanPrerequisites.h"
#include "TitanColor.h"

namespace Titan
{
	enum SceneBlendFactor
	{
		SBF_ONE,
		SBF_ZERO,
		SBF_DEST_COLOUR,
		SBF_SOURCE_COLOUR,
		SBF_ONE_MINUS_DEST_COLOUR,
		SBF_ONE_MINUS_SOURCE_COLOUR,
		SBF_DEST_ALPHA,
		SBF_SOURCE_ALPHA,
		SBF_ONE_MINUS_DEST_ALPHA,
		SBF_ONE_MINUS_SOURCE_ALPHA

	};


	enum SceneBlendOperation
	{
		SBO_ADD,
		SBO_SUBTRACT,
		SBO_REVERSE_SUBTRACT,
		SBO_MIN,
		SBO_MAX
	};
}
#endif