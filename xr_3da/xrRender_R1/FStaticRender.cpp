// CRender.cpp: implementation of the CRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
#include "..\fbasicvisual.h"
#include "..\fcached.h"
#include "..\CustomHUD.h"
#include "..\xr_object.h"
#include "lighttrack.h"

using	namespace		R_dsgraph;

CRender													RImplementation;

//////////////////////////////////////////////////////////////////////////
void					CRender::create					()
{
	::Device.Resources->SetHLSL_path("R1\\");
	Models						= xr_new<CModelPool>		();
	L_Dynamic					= xr_new<CLightR_Manager>	();

	PSLibrary.OnCreate			();
	PSLibrary.OnDeviceCreate	();
}
void					CRender::destroy				()
{
	xr_delete					(L_Dynamic);
	xr_delete					(Models);

	PSLibrary.OnDeviceDestroy	();
	PSLibrary.OnDestroy			();
}

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(IRenderable* parent)					{ return xr_new<CLightTrack>();			}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)			{ xr_delete(p);							}
IRender_Visual*			CRender::model_Create			(LPCSTR name)							{ return Models->Create(name);			}
IRender_Visual*			CRender::model_Create			(LPCSTR name, IReader* data)			{ return Models->Create(name,data);		}
IRender_Visual*			CRender::model_Duplicate		(IRender_Visual* V)						{ return Models->Instance_Duplicate(V);	}
void					CRender::model_Delete			(IRender_Visual* &V, BOOL bDiscard)		{ Models->Delete(V,bDiscard);			}
IRender_DetailModel*	CRender::model_CreateDM			(IReader*	F)
{
	CDetail*	D		= xr_new<CDetail> ();
	D->Load				(F);
	return D;
}
void					CRender::model_Delete			(IRender_DetailModel* & F)
{
	if (F)
	{
		CDetail*	D	= (CDetail*)F;
		D->Unload		();
		xr_delete		(D);
		F				= NULL;
	}
}
IRender_Visual*			CRender::model_CreatePE			(LPCSTR name)	
{ 
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);		VERIFY(SE);
	return				Models->CreatePE	(SE);
}

IRender_Visual*			CRender::model_CreateParticles	(LPCSTR name)	
{ 
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);
	if (SE) return		Models->CreatePE	(SE);
	else{
		PS::CPGDef*	SG	= PSLibrary.FindPGD	(name);		R_ASSERT(SG);
		return			Models->CreatePG	(SG);
	}
}

ref_shader				CRender::getShader				(int id)			{ VERIFY(id<int(Shaders.size()));	return Shaders[id];	}
IRender_Portal*			CRender::getPortal				(int id)			{ VERIFY(id<int(Portals.size()));	return Portals[id];	}
IRender_Sector*			CRender::getSector				(int id)			{ VERIFY(id<int(Sectors.size()));	return Sectors[id];	}
IRender_Sector*			CRender::getSectorActive		()					{ return pLastSector;									}
IRender_Visual*			CRender::getVisual				(int id)			{ VERIFY(id<int(Visuals.size()));	return Visuals[id];	}
D3DVERTEXELEMENT9*		CRender::getVB_Format			(int id)			{ VERIFY(id<int(DCL.size()));		return DCL[id].begin();	}
IDirect3DVertexBuffer9*	CRender::getVB					(int id)			{ VERIFY(id<int(VB.size()));		return VB[id];		}
IDirect3DIndexBuffer9*	CRender::getIB					(int id)			{ VERIFY(id<int(IB.size()));		return IB[id];		}
IRender_Target*			CRender::getTarget				()					{ return Target;										}

IRender_Light*			CRender::light_create			()					{ return L_DB->Create();								}
void					CRender::light_destroy			(IRender_Light* &L)	{ if (L) { L_DB->Destroy((light*)L); L=0; }				}

void					CRender::flush					()					{ r_dsgraph_render_graph	(0);						}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);								}

void					CRender::add_Visual				(IRender_Visual* V ){ add_leafs_Dynamic(V);									}
void					CRender::add_Geometry			(IRender_Visual* V ){ add_Static(V,View->getMask());						}
void					CRender::add_Wallmark			(ref_shader& S, const Fvector& P, float s, CDB::TRI* T)
{
	Wallmarks->AddWallmark	(T,P,&*S,s);
}
void					CRender::set_Object				(IRenderable*		O )	
{
	val_pObject				= O;		// NULL is OK, trust me :)
	L_Shadows->set_object	(O);
	L_Projector->set_object	(O);
	if (O)
	{
		VERIFY				(O->renderable.ROS);
		CLightTrack*		T = (CLightTrack*)O->renderable.ROS;
		T->ltrack			(O);
	}
}
void					CRender::ApplyObject			(IRenderable*		O )
{
	if (0==O)			return;
	VERIFY				(O->renderable.ROS);
	CLightTrack& LT		= *((CLightTrack*)O->renderable.ROS);

	// shadowing
	if ((LT.Shadowed_dwFrame==Device.dwFrame) && O->renderable_ShadowReceive())	
		RImplementation.L_Projector->setup	(LT.Shadowed_Slot);
}

// Misc
static	float			g_fGLOD, g_fFarSq, g_fPOWER;
float					g_fSCREEN;
float					g_fLOD,g_fLOD_scale=1.f;
static	BOOL			gm_Nearer	= 0;

IC		void			gm_SetNearer		(BOOL bNearer)
{
	if (bNearer	!= gm_Nearer)
	{
		gm_Nearer	= bNearer;
		if (gm_Nearer)	RImplementation.rmNear	();
		else			RImplementation.rmNormal();
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender	()
{
}

CRender::~CRender	()
{
}

extern float		r_ssaDISCARD;
extern float		r_ssaDONTSORT;
extern float		r_ssaLOD_A;
extern float		r_ssaLOD_B;
extern float		r_ssaHZBvsTEX;

IC bool				pred_sp_sort	(ISpatial* _1, ISpatial* _2)
{
	float	d1		= _1->spatial.center.distance_to_sqr(Device.vCameraPosition);
	float	d2		= _2->spatial.center.distance_to_sqr(Device.vCameraPosition);
	return	d1<d2;
}

void CRender::Calculate				()
{
	Device.Statistic.RenderCALC.Begin();

	// Transfer to global space to avoid deep pointer access
	IRender_Target* T				=	getTarget	();
	g_fFarSq						=	75.f;
	g_fFarSq						*=	g_fFarSq;
	g_fSCREEN						=	float(T->get_width()*T->get_height());
	g_fLOD							=	g_fLOD_scale;
	r_ssaDISCARD					=	_sqr(ps_r1_ssaDISCARD)/g_fSCREEN;
	r_ssaDONTSORT					=	_sqr(ps_r1_ssaDONTSORT)/g_fSCREEN;
	r_ssaLOD_A						=	_sqr(ps_r1_ssaLOD_A)/g_fSCREEN;
	r_ssaLOD_B						=	_sqr(ps_r1_ssaLOD_B)/g_fSCREEN;
	r_ssaHZBvsTEX					=	_sqr(ps_r1_ssaHZBvsTEX)/g_fSCREEN;

	// Frustum & HOM rendering
	ViewBase.CreateFromMatrix		(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= 0;
	HOM.Enable						();
	HOM.Render						(ViewBase);
	gm_SetNearer					(FALSE);

	// Detect camera-sector
	if (!vLastCameraPos.similar(Device.vCameraPosition,EPS_S)) 
	{
		CSector* pSector		= (CSector*)detectSector(Device.vCameraPosition);
		if (0==pSector) pSector = pLastSector;
		pLastSector = pSector;
		vLastCameraPos.set(Device.vCameraPosition);
	}

	// Check if camera is too near to some portal - if so force DualRender
	if (rmPortals) 
	{
		Fvector box_radius; box_radius.set(EPS_L*2,EPS_L*2,EPS_L*2);
		XRC.box_options	(0);
		XRC.box_query	(rmPortals,Device.vCameraPosition,box_radius);
		for (int K=0; K<XRC.r_count(); K++)
		{
			CPortal*	pPortal	= (CPortal*) rmPortals->get_tris()[XRC.r_begin()[K].id].dummy;
			pPortal->bDualRender = TRUE;
		}
	}

	// Main process
	marker	++;
	if (pLastSector)
	{
		// Traverse sector/portal structure
		PortalTraverser.traverse	
			(
			pLastSector,
			ViewBase,
			Device.vCameraPosition,
			Device.mFullTransform,
			CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA
			);

		// Determine visibility for static geometry hierrarhy
		for (u32 s_it=0; s_it<PortalTraverser.r_sectors.size(); s_it++)
		{
			CSector*	sector		= (CSector*)PortalTraverser.r_sectors[s_it];
			IRender_Visual*	root	= sector->root();
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
			{
				set_Frustum			(&(sector->r_frustums[v_it]));
				add_Geometry		(root);
			}
		}

		// Traverse object database
		g_SpatialSpace->q_frustum
			(
			ISpatial_DB::O_ORDERED,
			STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
			ViewBase
			);

		// Exact sorting order (front-to-back)
		lstRenderables.swap					(g_SpatialSpace->q_result);
		std::sort							(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

		// Determine visibility for dynamic part of scene
		set_Object							(0);
		g_pGameLevel->pHUD->Render_First	( );
		for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
		{
			ISpatial*	spatial		= lstRenderables[o_it];
			CSector*	sector		= (CSector*)spatial->spatial.sector;
			if	(0==sector)										continue;	// disassociated from S/P structure
			if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
			{
				CFrustum&	view	= sector->r_frustums[v_it];
				if (!view.testSphere_dirty(spatial->spatial.center,spatial->spatial.radius))	continue;


				if (spatial->spatial.type & STYPE_RENDERABLE)
				{
					// renderable
					IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
					if (0==renderable)	
					{
						// It may be an glow
						CGlow*		glow				= dynamic_cast<CGlow*>(spatial);
						VERIFY							(glow);
						L_Glows->add					(glow);
					} else {
						// Occlusion
						vis_data&		v_orig			= renderable->renderable.visual->vis;
						vis_data		v_copy			= v_orig;
						v_copy.box.xform				(renderable->renderable.xform);
						BOOL			bVisible		= HOM.visible(v_copy);
						v_orig.frame					= v_copy.frame;
						v_orig.hom_frame				= v_copy.hom_frame;
						v_orig.hom_tested				= v_copy.hom_tested;
						if (!bVisible)					break;	// exit loop on frustums

						// Rendering
						set_Object						(renderable);
						renderable->renderable_Render	();
						set_Object						(0);	//? is it needed at all
					}
				} else {
					VERIFY								(spatial->spatial.type & STYPE_LIGHTSOURCE);
					// lightsource
					light*			L					= dynamic_cast<light*>		(spatial);
					VERIFY								(L);
					L_DB->add_light						(L);
				}
				break;	// exit loop on frustums
			}
		}
		g_pGameLevel->pHUD->Render_Last						();	

		// Calculate miscelaneous stuff
		calc_DetailTexturing								();
		L_Shadows->calculate								();
		L_Projector->calculate								();
	}
	else
	{
		set_Object											(0);
		g_pGameLevel->pHUD->Render_First					();	
		g_pGameLevel->pHUD->Render_Last						();	
	}

	// End calc
	Device.Statistic.RenderCALC.End	();
}

void	CRender::rmNear		()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmFar		()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmNormal	()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}

void	CRender::Render		()
{
	Device.Statistic.RenderDUMP.Begin();

	// Target.set_gray				(.5f+_sin(Device.fTimeGlobal)/2.f);
	Target->Begin					();


	///////////////////////////???????????????????????????????????????????????????????????????/
	r_dsgraph_render_hud			();

	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea

	// Sorting by SSA and changes minimizations
		if (1==pr)			{
			RCache.set_xform_world	(Fidentity);
			Details->Render			(Device.vCameraPosition);

			g_pGamePersistent->Environment.RenderFirst	();

			RCache.set_xform_world	(Fidentity);
			Wallmarks->Render		();		// Wallmarks has priority as normal geometry

			RCache.set_xform_world	(Fidentity);
			L_Dynamic->render		();		// L_DB has priority the same as normal geom

			RCache.set_xform_world	(Fidentity);
			L_Shadows->render		();
		}

	// LODs
	r_dsgraph_render_lods	();
	r_dsgraph_render_sorted	();

	// Glows
	L_Glows->Render			();

	g_pGamePersistent->Environment.RenderLast	();

	// Postprocess, if necessary
	Target->End				();
	L_Projector->finalize	();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}
