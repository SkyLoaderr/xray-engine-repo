// CRender.h: interface for the CRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "render.h"

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

// defs
const int max_patches = 512;

// definition
class ENGINE_API CRender			:  public CRender_interface
{
private:
	// Dynamic scene graph
	SceneGraph::mapNormal_T							mapNormal[4];
	SceneGraph::mapMatrix_T							mapMatrix;
	SceneGraph::mapSorted_T							mapSorted;
	SceneGraph::vecPatches_T						vecPatches;

	vector<FCached*>								vecCached;
	vector<SceneGraph::mapNormalCodes::TNode*>		lstCodes;
	vector<SceneGraph::mapNormalTextures::TNode*>	lstTextures;
	vector<SceneGraph::mapNormalMatrices::TNode*>	lstMatrices;
	vector<SceneGraph::mapNormalConstants::TNode*>	lstConstants;

	CVertexStream*									vsPatches;

	// Sector detection and visibility
	CSector*										pLastSector;
	Fvector											vLastCameraPos;
	vector<CPortal>									Portals;
	vector<CSector*>								Sectors;
	CDB::MODEL*										rmPortals;
	CHOM											HOM;
	
	// Global vertex-buffer container
	vector<DWORD>									FVF;
	vector<IDirect3DVertexBuffer8*>					VB;
	vecGEO											Visuals;

	CLightDB_Static									L_DB;
	CLightPPA_Manager								L_Dynamic;
	CLightShadows									L_Shadows;
	CGlowManager									Glows;
	CWallmarksEngine								Wallmarks;
	CDetailManager									Details;
	CModelPool										Models;

	CRenderTarget									Target;			// Render-target
private:
	// Loading / Unloading
	void							LoadBuffers				(CStream	*fs);
	void							LoadVisuals				(CStream	*fs);
	void							LoadLights				(CStream	*fs);
	void							LoadPortals				(CStream	*fs);
	void							LoadSectors				(CStream	*fs);

	BOOL							add_Dynamic				(CVisual	*pVisual, DWORD planes);	// normal processing
	void							add_Static				(CVisual	*pVisual, DWORD planes);
	void							add_leafs_Dynamic		(CVisual	*pVisual);					// if detected node's full visibility
	void							add_leafs_Static		(CVisual	*pVisual);					// if detected node's full visibility
	void							InsertSG_Dynamic		(CVisual	*pVisual, Fvector& Center);
	void							InsertSG_Static			(CVisual	*pVisual);
	void							InsertSG_Cached			(CVisual	*pVisual);
public:
	// Loading / Unloading
	virtual	void					level_Load				();
	virtual void					level_Unload			();
	
	// Information
	virtual int						getVisualsCount			();
	virtual CPortal*				getPortal				(int id);
	virtual CSector*				getSector				(int id);
	virtual CVisual*				getVisual				(int id);
	virtual CSector*				detectSector			(Fvector& P);
	
	// Main 
	virtual void					set_Object				(CObject*	O	);
	virtual void					add_Visual				(CVisual*	V	);			// add visual leaf (no culling performed at all)
	virtual void					add_Lights				(vector<WORD> &V);
	virtual void					add_Glows				(vector<WORD> &V);
	virtual void					add_Patch				(Shader* S, Fvector& P1, float s, float a, BOOL bNearer);
	
	virtual void					Calculate				();
	virtual void					Render					();
	virtual void					Screenshot				();
	
	virtual void					rmNear					();
	virtual void					rmFar					();
	virtual void					rmNormal				();

	// Device dependance
	virtual void					OnDeviceDestroy			();
	virtual void					OnDeviceCreate			();

	// Constructor/destructor/loader
	CRender							();
	virtual ~CRender				();
};

#endif // !defined(AFX_FSTATICRENDER_H__CC9820C1_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
