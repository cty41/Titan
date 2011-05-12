#ifndef _TITAN_GEOTERRAIN_HH
#define _TITAN_GEOTERRAIN_HH

#include "TiPrerequisites.h"
#include "TiBaseTerrain.h"
#include "TiGeoTerrainDefs.h"


namespace Titan
{
#define TOTAL_DETAIL_LEVELS		4
#define TOTAL_SHAPES			16

	class GeoTerrainSection;

	class _DllExport GeoTerrain: public BaseTerrain
	{
	public:
		enum side_flags
		{
			Top = 0,
			Left,
			Right,
			Bottom,
			TotalSides
		};

		struct IndexBufferInfo: public GeneralAlloc
		{
			bool	isStrip;
			IndexBufferPtr	pIndex;

			IndexBufferInfo(IndexBufferPtr IndexPtr, bool isStrip)
				:pIndex(IndexPtr), isStrip(isStrip){}
		};
		struct IndexStruct
		{
			IndexBufferInfo*  tileBodies[TOTAL_SHAPES];
			IndexBufferInfo*  tileConnectors[TotalSides][TOTAL_DETAIL_LEVELS];
			IndexStruct()
			{
				memset(tileBodies, 0, sizeof(tileBodies));
				memset(tileConnectors, 0, sizeof(tileConnectors));
			}
		};

	public:
		GeoTerrain();

		virtual ~GeoTerrain();

		virtual void create(SceneNode* rootNode, TexturePtr heightMap, const AABB& worldBound, uint8 shift = 3);

		virtual void destroy();

		GeoTerrainSection*	getGeoSection(size_t index) const;
				
		const IndexStruct& getDetailLevelStruct(size_t index) const;

	protected:
		virtual void _createTerrainSections();

		IndexBufferPtr	_createIndexStructs(const GeoIndexData& srcData);

		void	_genDetailLevels();

	protected:
		IndexStruct	mDetailLevelStructs[TOTAL_DETAIL_LEVELS];
	};
}
#endif