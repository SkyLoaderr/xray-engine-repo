#pragma once

#include "..\PSLibrary.h"

#include "r2_types.h"
#include "r2_scenegraph.h"
#include "r2_rendertarget.h"

#include "hom.h"
#include "portal.h"
#include "detailmanager.h"
#include "modelpool.h"

#include "light_db.h"
#include "light_render_direct.h"

#include "common_binders.h"

// definition
class CRender	:	public IRender_interface
{
public:
	enum
	{
		PHASE_NORMAL	= 0,	// E[0]
		PHASE_SMAP_D	= 1,	// E[1]
		PHASE_SMAP_P	= 2,	// E[2]
		PHASE_SMAP_S	= 3		// E[3]
	};
	u32															phase;
	BOOL														b_nv3x;

	// Dynamic scene graph
	SceneGraph::mapNormal_T										mapNormal;
	SceneGraph::mapMatrix_T										mapMatrix;
	SceneGraph::mapSorted_T										mapSorted;
	SceneGraph::mapHUD_T										mapHUD;

	xr_vector<SceneGraph::mapNormalVS::TNode*>					nrmVS;
	xr_vector<SceneGraph::mapNormalPS::TNode*>					nrmPS;
	xr_vector<SceneGraph::mapNormalCS::TNode*>					nrmCS;
	xr_vector<SceneGraph::mapNormalStates::TNode*>				nrmStates;
	xr_vector<SceneGraph::mapNormalTextures::TNode*>			nrmTextures;
	xr_vector<SceneGraph::mapNormalTextures::TNode*>			nrmTexturesTemp;
	xr_vector<SceneGraph::mapNormalVB::TNode*>					nrmVB;

	xr_vector<SceneGraph::mapMatrixVS::TNode*>					matVS;
	xr_vector<SceneGraph::mapMatrixPS::TNode*>					matPS;
	xr_vector<SceneGraph::mapMatrixCS::TNode*>					matCS;
	xr_vector<SceneGraph::mapMatrixStates::TNode*>				matStates;
	xr_vector<SceneGraph::mapMatrixTextures::TNode*>			matTextures;
	xr_vector<SceneGraph::mapMatrixTextures::TNode*>			matTexturesTemp;
	xr_vector<SceneGraph::mapMatrixVB::TNode*>					matVB;

	// Sector detection and visibility
	CSector*													pLastSector;
	Fvector														vLastCameraPos;
	xr_vector<IRender_Portal*>									Portals;
	xr_vector<IRender_Sector*>									Sectors;
	CDB::MODEL*													rmPortals;
	CHOM														HOM;

	// Global vertex-buffer container
	typedef svector<D3DVERTEXELEMENT9,MAXD3DDECLLENGTH+1>		VertexDeclarator;
	xr_vector<VertexDeclarator>									DCL;
	xr_vector<IDirect3DVertexBuffer9*>							VB;
	xr_vector<IDirect3DIndexBuffer9*>							IB;
	xr_vector<IRender_Visual*>									Visuals;
	CPSLibrary													PSLibrary;

	CDetailManager												Details;
	CModelPool													Models;

	CRenderTarget												Target;			// Render-target
	CLight_DB													Lights;
	CLight_Render_Direct										LR;
	cl_binders													Binders;
private:
	// Loading / Unloading
	void							LoadBuffers					(IReader	*fs);
	void							LoadVisuals					(IReader	*fs);
	void							LoadLights					(IReader	*fs);
	void							LoadPortals					(IReader	*fs);
	void							LoadSectors					(IReader	*fs);
	void							LoadTrees					(IReader	*fs);

	BOOL							add_Dynamic					(IRender_Visual	*pVisual, u32 planes);		// normal processing
	void							add_Static					(IRender_Visual	*pVisual, u32 planes);
	void							add_leafs_Dynamic			(IRender_Visual	*pVisual);					// if detected node's full visibility
	void							add_leafs_Static			(IRender_Visual	*pVisual);					// if detected node's full visibility
	void							InsertSG_Dynamic			(IRender_Visual	*pVisual, Fvector& Center);
	void							InsertSG_Static				(IRender_Visual	*pVisual);

	void							flush_Models				();
	void							flush_LODs					();

	IRender_Sector*					detectSector				(Fvector& P, Fvector& D);
public:
	void							render_scenegraph			();
	void							render_hud					();
	void							render_smap_direct			(Fmatrix& mCombined);
	void							render_smap_sector			(CSector* S, Fmatrix& mCombined, Fvector& C);
public:
	// Loading / Unloading
	virtual void					create						();
	virtual void					destroy						();

	virtual	void					level_Load					();
	virtual void					level_Unload				();

	// Information
	virtual int						getVisualsCount				();
	virtual IRender_Portal*			getPortal					(int id);
	virtual IRender_Sector*			getSector					(int id);
	virtual IRender_Sector*			getSectorActive				();
	virtual IRender_Visual*			getVisual					(int id);
	virtual D3DVERTEXELEMENT9*		getVB_Format				(int id);
	virtual IDirect3DVertexBuffer9*	getVB						(int id);
	virtual IDirect3DIndexBuffer9*	getIB						(int id);
	virtual IRender_Sector*			detectSector				(Fvector& P);
	virtual IRender_Target*			getTarget					();

	// Main 
	virtual void					flush						();
	virtual void					set_Object					(IRenderable*		O	);
	virtual void					add_Visual					(IRender_Visual*	V	);			// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry				(IRender_Visual*	V	);			// add visual(s)	(all culling performed)
	virtual void					add_Wallmark				(ref_shader& S, const Fvector& P, float s, CDB::TRI* T);

	//
	virtual IBlender*				blender_create				(CLASS_ID cls);
	virtual void					blender_destroy				(IBlender* &);

	//
	virtual IRender_ObjectSpecific*	ros_create					(IRenderable*		parent);
	virtual void					ros_destroy					(IRender_ObjectSpecific* &);

	// Lighting
	virtual IRender_Light*			light_create				();
	virtual void					light_destroy				(IRender_Light* &);

	// Models
	virtual IRender_Visual*			model_CreatePS				(LPCSTR name, PS::SEmitter* E);
	virtual IRender_Visual*			model_CreateParticles		(LPCSTR name);
	virtual IRender_DetailModel*	model_CreateDM				(IReader* F);
	virtual IRender_Visual*			model_Create				(LPCSTR name);
	virtual IRender_Visual*			model_Create				(LPCSTR name, IReader* data);
	virtual IRender_Visual*			model_Duplicate				(IRender_Visual*	V);
	virtual void					model_Delete				(IRender_Visual* &	V, BOOL bDiscard);
	virtual void 					model_Delete				(IRender_DetailModel* & F);
	virtual void					model_Logging				(BOOL bEnable)				{ Models.Logging(bEnable);	}

	// Occlusion culling
	virtual BOOL					occ_visible					(vis_data&	V);
	virtual BOOL					occ_visible					(Fbox&		B);
	virtual BOOL					occ_visible					(sPoly&		P);

	// Main
	virtual void					Calculate					();
	virtual void					Render						();
	virtual void					RenderBox					(IRender_Sector* S, Fbox& BB, int sh);
	virtual void					Screenshot					(LPCSTR postfix, BOOL bSquare=FALSE);

	// Render mode
	virtual void					rmNear						();
	virtual void					rmFar						();
	virtual void					rmNormal					();

	// Device dependance
	virtual void					OnDeviceDestroy				();
	virtual void					OnDeviceCreate				();

	// Constructor/destructor/loader
	CRender							();
	virtual ~CRender				();
};

extern CRender						RImplementation;

// shader name prefixed with codepath
// MT unsafe
extern LPCSTR						r2p(LPCSTR name);
extern LPCSTR						r2v(LPCSTR name);
