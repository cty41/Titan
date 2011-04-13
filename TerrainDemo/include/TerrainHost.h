#ifndef __TERRAIN__HOST__HH
#define __TERRAIN__HOST__HH

#include "BaseHost.h"

class TerrainHost : public BaseHost
{
public:
	TerrainHost();

	~TerrainHost();

	virtual void destroy();

	virtual void render(float frameTime);
protected:

	virtual void loadResources();

	virtual void updateLogic(float frameTime);

protected:

	Titan::BaseTerrain*				mTerrain;
	Titan::ManualObject*			mManualObject;

};


#endif