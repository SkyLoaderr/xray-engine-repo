// FStaticRender.h: interface for the FStaticRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "FStaticVisibility.h"
#include "FVisualContainer.h"
#include "ShadowController.h"

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

#define	MAX_VISUALS			1024
#define	MAX_ALPHAVISUALS	512
#define	MAX_TEXTURES		256

struct _Texture {
	DWORD			Count;
	float			fLod		[MAX_VISUALS];
	FBasicVisual*	pVisual		[MAX_VISUALS];
	Fmatrix*		pMatrix		[MAX_VISUALS];
	Fvector*		pCenter		[MAX_VISUALS];
};
struct _AlphaList {
	float			fDist;		// flod must be calculated at render time :(
	FBasicVisual*	pV;
	Fmatrix*		pM;
	Fvector*		pC;

	__forceinline	bool operator < (_AlphaList &other) {
		// reverse sort order for "back to front" render
		return		fDist>other.fDist;
	}
};

class FStaticRender			:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
	float					fGLOD;

	char*					pLoadFolder;

	// 0-NormalStatic, 1-NormalDynamic
	_Texture				list			[2][MAX_TEXTURES ];
	_AlphaList				list_alpha		[MAX_ALPHAVISUALS];
	DWORD					dwALCount;		// number of items in alpha-list

	DWORD					dwTexCount;

	// Mobile visuals
	vector<FBasicVisual*>	mobileVisuals;
	vector<Fmatrix*>		mobileMatrix;
	vector<Fvector*>		mobilePosition;

public:
	FStaticVisibility*		pVisibility;
	FVisualContainer*		pVisuals;
	CShadowController*		pShadows;

	void Calculate();

	void AddToRender(FBasicVisual *pVisual, Fmatrix *pMatrix, Fvector *pCenter, float LOD	);
	void AddToAlpha	(FBasicVisual *pVisual, Fmatrix *pMatrix, Fvector *pCenter, float dist	);
	void InitSorter();
	void Prepare();
	void Render();

	// Mobile objects
	void AddDynamic					(FBasicVisual *pVisual, Fmatrix *m, Fvector *p);
	FBasicVisual* RegisterDynamic	(char *name, Fmatrix *m, Fvector *p, bool bDuplicate=false);
	void UnregisterDynamic			(Fmatrix *m);

	// Constructor/destructor/loader
	FStaticRender();
	~FStaticRender();
	void Load(char *folder);

	// Device dependance
	virtual void OnDeviceDestroy();
	virtual void OnDeviceCreate	();
};

#endif // !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
