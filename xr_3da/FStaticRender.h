// CRender.h: interface for the CRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "FLightsController.h"
#include "scenegraph.h"
#include "glowmanager.h"
#include "wallmarksengine.h"
#include "portal.h"
#include "modelpool.h"
#include "fstaticrender_types.h"
#include "detailmanager.h"
#include "fstaticrender_rendertarget.h"
#include "hom.h"
#include "lightShadows.h"
#include "lightPPA.h"

// refs
class ENGINE_API	CStream;
class ENGINE_API	FCached;
class ENGINE_API	FBasicVisual;

// defs
const int max_patches = 512;

// definition
class ENGINE_API CRender			:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	// Dynamic scene graph
	Fmatrix*						pTransform;
	int								iLightLevel;
	SceneGraph::mapNormal_T			mapNormal[4];
	SceneGraph::mapMatrix_T			mapMatrix;
	SceneGraph::mapSorted_T			mapSorted;
	SceneGraph::vecPatches_T		vecPatches;

	CList<FCached*>									vecCached;
	CList<SceneGraph::mapNormalCodes::TNode*>		lstCodes;
	CList<SceneGraph::mapNormalTextures::TNode*>	lstTextures;
	CList<SceneGraph::mapNormalMatrices::TNode*>	lstMatrices;
	CList<SceneGraph::mapNormalConstants::TNode*>	lstConstants;

	CVertexStream*					vsPatches;

	// Sector detection
	CDB::MODEL*						rmPortals;
	CSector*						pLastSector;
	Fvector							vLastCameraPos;
public:
	// Global vertex-buffer container
	vector<DWORD>					FVF;
	vector<IDirect3DVertexBuffer8*>	VB;

	vecGEO							Visuals;
	CLightDB_Static					L_DB;
	CLightPPA_Manager				L_Dynamic;
	CLightShadows					L_Shadows;
	CGlowManager					Glows;
	CWallmarksEngine				Wallmarks;
	CDetailManager					Details;
	CModelPool						Models;

	CRenderTarget					Target;			// Render-target

	vector<CPortal>					Portals;
	vector<CSector*>				Sectors;
	CFrustum						ViewBase;
	CFrustum*						View;
	CHOM							HOM;

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
	IC void	set_Frustum				(CFrustum* O)			{ VERIFY(O); View = O;			}
	IC void	set_Transform			(Fmatrix* pM)			{ VERIFY(pM); pTransform = pM;	}
	IC void set_Object				(CObject* O)			{ L_Shadows.set_object(O);		}
	IC void set_LightLevel			(int L)					{ iLightLevel = L;				}
	IC void add_Lights				(vector<WORD> &V)		{ L_DB.add_sector_lights(V);	}
	IC void add_Glows				(vector<WORD> &V)		{ Glows.add(V);					}

	void	Calculate				();

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
	BOOL	add_Dynamic				(FBasicVisual	*pVisual, DWORD planes);	// normal processing
	void	add_Static				(FBasicVisual	*pVisual, DWORD planes);
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
