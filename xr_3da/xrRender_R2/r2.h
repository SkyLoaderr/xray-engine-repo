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
		PHASE_SMAP_P	= 2		// E[2]
	};
	u32														phase;

	// Dynamic scene graph
	SceneGraph::mapNormal_T									mapNormal;
	SceneGraph::mapMatrix_T									mapMatrix;
	SceneGraph::mapSorted_T									mapSorted;
	SceneGraph::mapHUD_T									mapHUD;

	vector<SceneGraph::mapNormalVS::TNode*>					nrmVS;
	vector<SceneGraph::mapNormalPS::TNode*>					nrmPS;
	vector<SceneGraph::mapNormalCS::TNode*>					nrmCS;
	vector<SceneGraph::mapNormalStates::TNode*>				nrmStates;
	vector<SceneGraph::mapNormalTextures::TNode*>			nrmTextures;
	vector<SceneGraph::mapNormalTextures::TNode*>			nrmTexturesTemp;
	vector<SceneGraph::mapNormalVB::TNode*>					nrmVB;

	vector<SceneGraph::mapMatrixVS::TNode*>					matVS;
	vector<SceneGraph::mapMatrixPS::TNode*>					matPS;
	vector<SceneGraph::mapMatrixCS::TNode*>					matCS;
	vector<SceneGraph::mapMatrixStates::TNode*>				matStates;
	vector<SceneGraph::mapMatrixTextures::TNode*>			matTextures;
	vector<SceneGraph::mapMatrixTextures::TNode*>			matTexturesTemp;
	vector<SceneGraph::mapMatrixVB::TNode*>					matVB;

	// Sector detection and visibility
	CSector*												pLastSector;
	Fvector													vLastCameraPos;
	vector<IRender_Portal*>									Portals;
	vector<IRender_Sector*>									Sectors;
	CDB::MODEL*												rmPortals;
	CHOM													HOM;

	// Global vertex-buffer container
	typedef svector<D3DVERTEXELEMENT9,MAX_FVF_DECL_SIZE>	VertexDeclarator;
	vector<VertexDeclarator>								DCL;
	vector<IDirect3DVertexBuffer9*>							VB;
	vector<IDirect3DIndexBuffer9*>							IB;
	vector<IVisual*>										Visuals;
	CPSLibrary												PSystems;

	CDetailManager											Details;
	CModelPool												Models;

	CRenderTarget											Target;			// Render-target
	CLight_DB												Lights;
	CLight_Render_Direct									LR_Direct;
	cl_binders												Binders;
private:
	// Loading / Unloading
	void							LoadBuffers				(IReader	*fs);
	void							LoadVisuals				(IReader	*fs);
	void							LoadLights				(IReader	*fs);
	void							LoadPortals				(IReader	*fs);
	void							LoadSectors				(IReader	*fs);
	void							LoadTrees				(IReader	*fs);

	BOOL							add_Dynamic				(IVisual	*pVisual, u32 planes);		// normal processing
	void							add_Static				(IVisual	*pVisual, u32 planes);
	void							add_leafs_Dynamic		(IVisual	*pVisual);					// if detected node's full visibility
	void							add_leafs_Static		(IVisual	*pVisual);					// if detected node's full visibility
	void							InsertSG_Dynamic		(IVisual	*pVisual, Fvector& Center);
	void							InsertSG_Static			(IVisual	*pVisual);

	void							flush_Models			();
	void							flush_LODs				();

public:
	void							render_scenegraph		();
	void							render_hud				();
	void							render_smap_direct		(Fmatrix& mCombined);
public:
	// Loading / Unloading
	virtual	void					level_Load				();
	virtual void					level_Unload			();

	// Information
	virtual int						getVisualsCount			();
	virtual IRender_Portal*			getPortal				(int id);
	virtual IRender_Sector*			getSector				(int id);
	virtual IRender_Sector*			getSectorActive			();
	virtual IVisual*				getVisual				(int id);
	virtual D3DVERTEXELEMENT9*		getVB_Format			(int id);
	virtual IDirect3DVertexBuffer9*	getVB					(int id);
	virtual IDirect3DIndexBuffer9*	getIB					(int id);
	virtual IRender_Sector*			detectSector			(Fvector& P);
	virtual IRender_Target*			getTarget				();

	// Main 
	virtual void					flush					();
	virtual void					set_Object				(CObject*	O	);
	virtual void					add_Visual				(IVisual*	V	);			// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry			(IVisual*	V	);			// add visual(s)	(all culling performed)
	virtual void					add_Lights				(vector<WORD> &V);
	virtual void					add_Glows				(vector<WORD> &V);
	virtual void					add_Patch				(Shader* S, const Fvector& P1, float s, float a, BOOL bNearer);
	virtual void					add_Wallmark			(Shader* S, const Fvector& P, float s, CDB::TRI* T);

	//
	virtual CBlender*				blender_create			(CLASS_ID cls);
	virtual void					blender_destroy			(CBlender* &);

	//
	virtual IRender_ObjectSpecific*	ros_create				(CObject* parent);
	virtual void					ros_destroy				(IRender_ObjectSpecific* &);

	// Lighting
	virtual IRender_Light*			light_create			();
	virtual void					light_destroy			(IRender_Light* &);
	virtual void					L_select				(Fvector &pos, float fRadius, vector<xrLIGHT*>&	dest);

	// Models
	virtual IVisual*				model_CreatePS			(LPCSTR name, PS::SEmitter* E);
	virtual IVisual*				model_CreatePG			(LPCSTR name);
	virtual IRender_DetailModel*	model_CreateDM			(IReader*	F);
	virtual IVisual*				model_Create			(LPCSTR name);
	virtual IVisual*				model_Create			(LPCSTR name, IReader* data);
	virtual IVisual*				model_Duplicate			(IVisual*	V);
	virtual void					model_Delete			(IVisual* &	V);
	virtual void 					model_Delete			(IRender_DetailModel* & F);

	// Occlusion culling
	virtual BOOL					occ_visible				(vis_data&	V);
	virtual BOOL					occ_visible				(Fbox&		B);
	virtual BOOL					occ_visible				(sPoly&		P);

	// Main
	virtual void					Calculate				();
	virtual void					Render					();
	virtual void					RenderBox				(IRender_Sector* S, Fbox& BB, int sh);
	virtual void					Screenshot				(LPCSTR postfix, BOOL bSquare=FALSE);

	// Render mode
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

extern CRender						RImplementation;
