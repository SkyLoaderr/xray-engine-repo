// CRender.h: interface for the CRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "FLightsController.h"
#include "LightPPA.h"
#include "fbasicvisual.h"
#include "scenegraph.h"
#include "glowmanager.h"
#include "occluder.h"
#include "wallmarksengine.h"
#include "portal.h"
#include "modelpool.h"
#include "fstaticrender_types.h"
#include "detailmanager.h"

// refs
class ENGINE_API CStream;
class ENGINE_API FCached;
namespace RAPID { class ENGINE_API Model; };

// defs
const int max_patches = 512;

// definition
class ENGINE_API CRender			:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	float							fGLOD;
	float							fPOWER;

	// Dynamic scene graph
	Fmatrix*						pTransform;
	int								iLightLevel;
	SceneGraph::mapNormal_T			mapNormal[4];
	SceneGraph::mapMatrix_T			mapMatrix;
	SceneGraph::mapSorted_T			mapSorted;
	SceneGraph::mapLine_T			mapLines;
	SceneGraph::vecPatches_T		vecPatches;

	CVertexStream*					vsPatches;

	// Sector detection
	RAPID::Model*					rmPortals;
	CSector*						pLastSector;
	Fvector							vLastCameraPos;
public:
	CList<FCached*>						vecCached;
	CList<SceneGraph::mapNormal_Node*>	vecNormalNodes;

	// Global vertex-buffer container
	vector<DWORD>					FVF;
	vector<IDirect3DVertexBuffer8*>	VB;

	vecGEO							Visuals;
	vector<CPortal>					Portals;
	vector<CSector*>				Sectors;
	CLightsController				Lights;
	CLightPPA_Manager				Lights_Dynamic;
	CGlowManager					Glows;
	CWallmarksEngine				Wallmarks;
	CDetailManager					Details;
	CModelPool						Models;

	CFrustum						ViewBase;
	COccluderSystem*				View;

	// Loading / Unloading
	void	level_Load				();
	void	level_Unload			();

	void	LoadBuffers				(CStream *fs);
	void	LoadVisuals				(CStream *fs);
	void	LoadLights				(CStream *fs);
	void	LoadPortals				(CStream *fs);
	void	LoadSectors				(CStream *fs);

	// Information
	int				getVisualsCount	()			{ return Visuals.size(); }
	CPortal*		getPortal(WORD id)			{ VERIFY(id<Portals.size()); return &Portals[id]; }
	CSector*		getSector(WORD id)			{ VERIFY(id<Sectors.size()); return Sectors[id]; }
	FBasicVisual*	getVisual(DWORD id)			{ VERIFY(id<Visuals.size()); return Visuals[id]; }
	CSector*		detectSector(Fvector& P);

	// Main 
	IC void	set_Occluders			(COccluderSystem* O)	{ VERIFY(O); View = O; }
	IC void	set_Transform			(Fmatrix* pM)			{ VERIFY(pM); pTransform = pM; }
	IC void set_LightLevel			(int L)					{ iLightLevel = L; }
	IC void add_Lights				(vector<WORD> &V)		{ Lights.add_sector_lights(V);	}
	IC void add_Glows				(vector<WORD> &V)		{ Glows.add(V);		}

	void	Calculate				();

	void	add_Line				(Fvector& P1, Fvector& P2, float w, DWORD c, Shader* S)
	{
		SceneGraph::mapLine_Node*	N = mapLines.insert(S);
		SceneGraph::_LineItem L;	L.P1 = P1; L.P2 = P2; L.w = w; L.C = c;
		N->val.push_back			(L);
	}
	void	add_Patch				(Shader* S, Fvector& P1, float s, float a, BOOL bNearer)
	{
		vecPatches.push_back(SceneGraph::_PatchItem());
		SceneGraph::_PatchItem& P = vecPatches.back();
		P.S		= S;
		P.P		= P1;
		P.size	= s;
		P.angle = a;
		P.nearer= bNearer;
	}
	BOOL	add_Dynamic				(FBasicVisual	*pVisual, CVisiCache C);		// normal processing
	void	add_Static				(FBasicVisual	*pVisual, CVisiCache C);
	void	add_leafs_Dynamic		(FBasicVisual	*pVisual);					// if detected node's full visibility
	void	add_leafs_Static		(FBasicVisual	*pVisual);					// if detected node's full visibility
	void	InsertSG_Dynamic		(FBasicVisual	*pVisual, Fvector& Center);
	void	InsertSG_Static			(FBasicVisual	*pVisual);
	void	InsertSG_Cached			(FCached		*pVisual);

	void	rmNear					();
	void	rmFar					();
	void	rmNormal				();

	void	flush_Static			();
	void	flush_Models			();
	void	flush_Patches			();
	void	Render					();

	// Screenshots
	void	MakeScreenshot			();

	// Constructor/destructor/loader
	CRender							();
	~CRender						();

	// Device dependance
	virtual void OnDeviceDestroy	();
	virtual void OnDeviceCreate		();
};

extern ENGINE_API CRender	Render;

#endif // !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
