#ifndef __TITAN_GEOTERRAIN_SECTION__HH
#define __TITAN_GEOTERRAIN_SECTION__HH

#include "TiPrerequisites.h"
#include "TiBaseTerrainSection.h"
#include "TiGeoTerrain.h"

namespace Titan
{

	class _DllExport GeoTerrainSection: public BaseTerrainSection
	{
	public:
		GeoTerrainSection();

		virtual ~GeoTerrainSection();

		virtual void create(BaseTerrain* creator, uint16 sectorX, uint16 sectorZ, uint16 heightMapX, uint16 heightMapZ, uint16 xVerts, uint16 zVerts, const Rect2D& worldRect);

		virtual void _updateRenderQueue(RenderQueue* queue, Camera* cam);

		int	getCurrentLod()const { return mCurrentLod; }


	protected:
		void	_calcCurrentLod(Camera* cam);

		void	_calcErrorMetricsTable();

		void	_createConnectorRend();
	protected:
		int		mCurrentLod;
		float	mErrorMetrics[TOTAL_DETAIL_LEVELS];
		TerrainSectionRend	mConnectorRends[GeoTerrain::TotalSides];
	};
}
#endif