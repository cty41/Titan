#ifndef _TITAN_PERLIN_H
#define _TITAN_PERLIN_H

#include "TiPrerequisites.h"
#include "TitanMath.h"
#include "TiVector2.h"
#include "TiVector4.h"

namespace Titan
{

	class _DllExport PerlinNoise : public GeneralAlloc
	{
	public:
		enum
		{
			k_tableSize = 256,
			k_tableMask = k_tableSize-1,
		};

		PerlinNoise();
		~PerlinNoise();

		float noise(int x, int y, float scale);
		float noise(float x, float y, float scale);

	private:

		Vector2 m_vecTable[k_tableSize];
		unsigned char m_lut[k_tableSize];

		// Private Functions...
		void setup();
		const Vector2& getVec(int x, int y)const;
	};

	struct PerlinPacket
	{
		PerlinNoise perlin;
		float scale;
		int octaves;
		float falloff;
		uint16 sizeX;
		uint16 sizeY;
	};

} 

#endif  // end of file      ( PerlinNoise.h )

