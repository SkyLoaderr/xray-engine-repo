#pragma once

#define rsDrawLights		0x0001
#define rsDrawGrid			0x0002

struct _FpsController {
	// General params
	float	fMinFPS;			//	Minimal possible FPS'es
	float	fMaxFPS;			//	Maximal possible FPS'es
	float	fGeometryLOD;		//	User prefferred LOD

	// Calculated values
	float	fScaleGLOD;			//	Geometry LOD scale
};

extern	_FpsController		QualityControl;

typedef vector<FBasicVisual*>	vecGEO;
typedef vecGEO::iterator		vecGEOIT;

