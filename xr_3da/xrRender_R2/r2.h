#pragma once

#include "r__dsgraph_structure.h"

#include "PSLibrary.h"

#include "r2_types.h"
#include "r2_rendertarget.h"

#include "hom.h"
#include "portal.h"
#include "detailmanager.h"
#include "modelpool.h"

#include "light_db.h"
#include "light_render_direct.h"

#include "common_binders.h"

// definition
class CRender													:	public R_dsgraph_structure
{
public:
	enum
	{
		PHASE_NORMAL	= 0,	// E[0]
		PHASE_SMAP_D	= 1,	// E[1]
		PHASE_SMAP_P	= 2,	// E[2]
		PHASE_SMAP_S	= 3		// E[3]
	};
	BOOL														b_nv3x;

public:
	// Sector detection and visibility
	CSector*													pLastSector;
	Fvector														vLastCameraPos;
	xr_vector<IRender_Portal*>									Portals;
	xr_vector<IRender_Sector*>									Sectors;
	CDB::MODEL*													rmPortals;
	CHOM														HOM;

	// Global vertex-buffer container
	xr_vector<ref_shader>										Shaders;
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

	virtual		ShaderElement*		rimp_select_sh_static		(IRender_Visual	*pVisual, float cdist_sq);
	virtual		ShaderElement*		rimp_select_sh_dynamic		(IRender_Visual	*pVisual, float cdist_sq);

	IRender_Sector*					rimp_detectSector			(Fvector& P, Fvector& D);
	void							render_smap_direct			(Fmatrix& mCombined);
public:
	IRender_Portal*					getPortal					(int id);
	IRender_Sector*					getSectorActive				();
	IRender_Visual*					model_CreatePE				(LPCSTR name);
	IRender_Sector*					detectSector				(Fvector& P, Fvector& D);
public:
	// Loading / Unloading
	virtual void					create						();
	virtual void					destroy						();

	virtual	void					level_Load					();
	virtual void					level_Unload				();

	// Information
	virtual LPCSTR					getShaderPath				()									{ return "xr2_r3xx\\";	}
	virtual ref_shader				getShader					(int id);
	virtual IRender_Sector*			getSector					(int id);
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
	virtual void					add_Wallmark				(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);

	//
	virtual IBlender*				blender_create				(CLASS_ID cls);
	virtual void					blender_destroy				(IBlender* &);

	//
	virtual IRender_ObjectSpecific*	ros_create					(IRenderable*		parent);
	virtual void					ros_destroy					(IRender_ObjectSpecific* &);

	// Lighting
	virtual IRender_Light*			light_create				();
	virtual void					light_destroy				(IRender_Light* &);
	virtual IRender_Glow*			glow_create					();
	virtual void					glow_destroy				(IRender_Glow* &);

	// Models
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
	virtual void					Screenshot					(LPCSTR postfix, BOOL bSquare=FALSE);

	// Render mode
	virtual void					rmNear						();
	virtual void					rmFar						();
	virtual void					rmNormal					();

	// Constructor/destructor/loader
	CRender							();
	virtual ~CRender				();
};

extern CRender						RImplementation;
