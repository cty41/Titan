#ifndef _TITAN_RENDERDATA_HH
#define _TITAN_RENDERDATA_HH

#include "TitanPrerequisites.h"
#include "VertexIndexBufferData.h"

namespace Titan
{
	class _DllExport RenderData
	{
	public:
		enum OperationType {
			/// A list of points, 1 vertex per point
			OT_POINT_LIST = 1,
			/// A list of lines, 2 vertices per line
			OT_LINE_LIST = 2,
			/// A strip of connected lines, 1 vertex per line plus 1 start vertex
			OT_LINE_STRIP = 3,
			/// A list of triangles, 3 vertices per triangle
			OT_TRIANGLE_LIST = 4,
			/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
			OT_TRIANGLE_STRIP = 5,
			/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
			OT_TRIANGLE_FAN = 6
		};

		VertexData*		vertexData;

		IndexData*		indexData;

		OperationType	operationType;

		bool			useIndex;

		RenderData()
			: vertexData(0), operationType(OT_TRIANGLE_LIST),
			indexData(0), useIndex(true)
		{
		}

		~RenderData()
		{
			if(vertexData)
			{
				TITAN_DELETE vertexData;
				vertexData = NULL;
			}
			if (indexData)
			{
				TITAN_DELETE indexData;
				indexData = NULL;
			}
		}


	};
}
#endif