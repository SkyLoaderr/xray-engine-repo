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
#include "..\fmesh.h"

using	namespace		R_dsgraph;

CRender													RImplementation;

//////////////////////////////////////////////////////////////////////////
ShaderElement*			CRender::rimp_select_sh_dynamic	(IRender_Visual	*pVisual, float cdist_sq)
{
#if		RENDER==R_R1
	switch (phase)		{
	case PHASE_NORMAL:	return (RImplementation.L_Projector->shadowing()?pVisual->hShader->E[SE_R1_NORMAL_HQ]:pVisual->hShader->E[SE_R1_NORMAL_LQ])._get();
	case PHASE_POINT:	return pVisual->hShader->E[SE_R1_LPOINT]._get();
	case PHASE_SPOT:	return pVisual->hShader->E[SE_R1_LSPOT]._get();
	default:			NODEFAULT;
	}
#elif	RENDER==R_R2
	return pVisual->hShader->E[RImplementation.phase]._get();
#endif
}
//////////////////////////////////////////////////////////////////////////
ShaderElement*			CRender::rimp_select_sh_static	(IRender_Visual	*pVisual, float cdist_sq)
{
#if		RENDER==R_R1
	switch (phase)		{
	case PHASE_NORMAL:	return (((_sqrt(cdist_sq)-pVisual->vis.sphere.R)<20)?pVisual->hShader->E[SE_R1_NORMAL_HQ]:pVisual->hShader->E[SE_R1_NORMAL_LQ])._get();
	case PHASE_POINT:	return pVisual->hShader->E[SE_R1_LPOINT]._get();
	case PHASE_SPOT:	return pVisual->hShader->E[SE_R1_LSPOT]._get();
	default:			NODEFAULT;
	}
#elif	RENDER==R_R2
	return pVisual->hShader->E[RImplementation.phase]._get();
#endif
}

//////////////////////////////////////////////////////////////////////////
void					CRender::create					()
{
	// c-setup
	::Device.Resources->RegisterConstantSetup("L_dynamic_pos",		&r1_dlight_binder_PR);
	::Device.Resources->RegisterConstantSetup("L_dynamic_color",	&r1_dlight_binder_color);
	::Device.Resources->RegisterConstantSetup("L_dynamic_xform",	&r1_dlight_binder_xform);

	// distortion
	u32		v_dev	= CAP_VERSION(HW.Caps.raster_major, HW.Caps.raster_minor);
	u32		v_need	= CAP_VERSION(1,4);
	if ( v_dev >= v_need )						o.distortion = TRUE;
	else										o.distortion = FALSE;
	if (strstr(Core.Params,"-nodistort"))		o.distortion = FALSE;
	Msg				("* distortion: %s, dev(%d),need(%d)",o.distortion?"used":"unavailable",v_dev,v_need);
	m_skinning					= -1;

	// disasm
	o.disasm					= (strstr(Core.Params,"-disasm"))?		TRUE	:FALSE	;

	//
	Models						= xr_new<CModelPool>		();
	L_Dynamic					= xr_new<CLightR_Manager>	();
	PSLibrary.OnCreate			();
}
void					CRender::destroy				()
{
	PSLibrary.OnDestroy			();
	xr_delete					(L_Dynamic);
	xr_delete					(Models);
}
void					CRender::reset_begin			()
{
	xr_delete			(Target);
}
void					CRender::reset_end				()
{
	Target			=	xr_new<CRenderTarget>			();
}

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(IRenderable* parent)					{ return xr_new<CROS_impl>();			}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)			{ xr_delete(p);							}
IRender_Visual*			CRender::model_Create			(LPCSTR name, IReader* data)			{ return Models->Create(name,data);		}
IRender_Visual*			CRender::model_CreateChild		(LPCSTR name, IReader* data)			{ return Models->CreateChild(name,data);}
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
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);		R_ASSERT3(SE,"Particle effect doesn't exist",name);
	return				Models->CreatePE	(SE);
}

IRender_Visual*			CRender::model_CreateParticles	(LPCSTR name)	
{ 
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);
	if (SE) return		Models->CreatePE	(SE);
	else{
		PS::CPGDef*	SG	= PSLibrary.FindPGD	(name);		R_ASSERT3(SG,"Particle effect or group doesn't exist",name);
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
FSlideWindowItem*		CRender::getSWI					(int id)			{ VERIFY(id<int(SWIs.size()));		return &SWIs[id];	}

IRender_Light*			CRender::light_create			()					{ return L_DB->Create();								}
void					CRender::light_destroy			(IRender_Light* &L)	{ if (L) { L_DB->Destroy((light*)L); L=0; }				}

IRender_Glow*			CRender::glow_create			()					{ return xr_new<CGlow>();								}
void					CRender::glow_destroy			(IRender_Glow* &L)	{ xr_delete(L);											}		

void					CRender::flush					()					{ r_dsgraph_render_graph	(0);						}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);								}

void					CRender::add_Visual				(IRender_Visual* V ){ add_leafs_Dynamic(V);									}
void					CRender::add_Geometry			(IRender_Visual* V ){ add_Static(V,View->getMask());						}
void					CRender::add_StaticWallmark		(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* verts)
{
	VERIFY2							(_valid(P) && _valid(s) && T && verts && (s>EPS_L), "Invalid static wallmark params");
	Wallmarks->AddStaticWallmark	(T,verts,P,&*S,s);
}
void					CRender::add_SkeletonWallmark	(CSkeletonWallmark* wm)
{
	Wallmarks->AddSkeletonWallmark				(wm);
}
void					CRender::add_SkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
	Wallmarks->AddSkeletonWallmark				(xf, obj, sh, start, dir, size);
}
void					CRender::add_Occluder			(Fbox2&	bb_screenspace	)
{
	VERIFY					(_valid(bb_screenspace));
	HOM.occlude				(bb_screenspace);
}
void					CRender::set_Object				(IRenderable*		O )	
{
	val_pObject				= O;		// NULL is OK, trust me :)
	if (PHASE_NORMAL==phase)	{
		L_Shadows->set_object	(O);
		L_Projector->set_object	(O);
	} else {
		L_Shadows->set_object	(0);
		L_Projector->set_object	(0);
	}
}
void					CRender::ApplyObject			(IRenderable*		O )
{
	if (0==O)			return;
	if (PHASE_NORMAL==phase)	{
		VERIFY				(O->renderable.ROS);
		CROS_impl& LT		= *((CROS_impl*)O->renderable.ROS);

		// shadowing
		if ((LT.shadow_recv_frame==Device.dwFrame) && O->renderable_ShadowReceive())	
			RImplementation.L_Projector->setup	(LT.shadow_recv_slot);
	}
}

// Misc
float					g_fGLOD, g_fFarSq, g_fPOWER;
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
	float	fov_factor				=	90.f / Device.fFOV;
	g_fFarSq						=	75.f;
	g_fFarSq						*=	g_fFarSq;
	g_fSCREEN						=	float(T->get_width()*T->get_height())*fov_factor;
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
	phase							= PHASE_NORMAL;

	// Detect camera-sector
	if (!vLastCameraPos.similar(Device.vCameraPosition,EPS_S)) 
	{
		CSector* pSector		= (CSector*)detectSector(Device.vCameraPosition);
		if (0==pSector) pSector = pLastSector;
		pLastSector				= pSector;
		vLastCameraPos.set		(Device.vCameraPosition);
	}

	// Check if camera is too near to some portal - if so force DualRender
	if (rmPortals) 
	{
		Fvector box_radius;		box_radius.set(EPS_L*2,EPS_L*2,EPS_L*2);
		Sectors_xrc.box_options	(0);
		Sectors_xrc.box_query	(rmPortals,Device.vCameraPosition,box_radius);
		for (int K=0; K<Sectors_xrc.r_count(); K++)
		{
			CPortal*	pPortal		= (CPortal*) Portals[rmPortals->get_tris()[Sectors_xrc.r_begin()[K].id].dummy];
			pPortal->bDualRender	= TRUE;
		}
	}
	//
	L_DB->Update	();

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
			lstRenderables,
			ISpatial_DB::O_ORDERED,
			STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
			ViewBase
			);

		// Exact sorting order (front-to-back)
		std::sort							(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

		// Determine visibility for dynamic part of scene
		set_Object							(0);
		g_pGameLevel->pHUD->Render_First	( );	// R1 shadows
		g_pGameLevel->pHUD->Render_Last		( );	
		u32 uID_LTRACK						= 0xffffffff;
		if (phase==PHASE_NORMAL)			{
			uLastLTRACK	++;
			if (lstRenderables.size())		uID_LTRACK	= uLastLTRACK%lstRenderables.size();
		}
		for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
		{
			ISpatial*	spatial		= lstRenderables[o_it];		spatial->spatial_updatesector	();
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
					IRenderable*	renderable		= spatial->dcast_Renderable	();
					if (0==renderable)	{
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
						v_orig.accept_frame				= v_copy.accept_frame;
						v_orig.marker					= v_copy.marker;
						v_orig.hom_frame				= v_copy.hom_frame;
						v_orig.hom_tested				= v_copy.hom_tested;
						if (!bVisible)					break;	// exit loop on frustums

						// rendering
						if (o_it==uID_LTRACK)	{
							// track lighting environment
							VERIFY				(renderable->renderable.ROS);
							CROS_impl*		T = (CROS_impl*)renderable->renderable.ROS;
							T->ltrack			(renderable);
						}
						set_Object						(renderable);
						renderable->renderable_Render	();
						set_Object						(0);	//? is it needed at all
					}
				} else {
					VERIFY								(spatial->spatial.type & STYPE_LIGHTSOURCE);
					// lightsource
					light*			L					= (light*)	spatial->dcast_Light	();
					VERIFY								(L);
					vis_data&		vis					= L->get_homdata();
					if	(HOM.visible(vis))	L_DB->add_light	(L);
				}
				break;	// exit loop on frustums
			}
		}

		// Calculate miscelaneous stuff
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

	// Begin
	Target->Begin								();
	o.vis_intersect								= FALSE			;
	phase										= PHASE_NORMAL	;
	r_dsgraph_render_hud						();				// hud
	r_dsgraph_render_graph						(0);			// normal level
	Details->Render								();				// grass / details
	g_pGamePersistent->Environment.RenderFirst	();				// sky / sun
	r_pmask										(true,false);	// disable priority "1"
	o.vis_intersect								= TRUE			;
	L_Dynamic->render							();				// addititional light sources
	Wallmarks->Render							();				// wallmarks has priority as normal geometry
	o.vis_intersect								= FALSE			;
	phase										= PHASE_NORMAL	;
	r_pmask										(true,true);	// enable priority "0" and "1"
	L_Shadows->render							();				// ... and shadows
	r_dsgraph_render_lods						();				// lods
	r_dsgraph_render_graph						(1);			// normal level, secondary priority
	r_dsgraph_render_sorted						();				// strict-sorted geoms
	L_Glows->Render								();				// glows
	g_pGamePersistent->Environment.RenderFlares	();				// rain/lens-flares/thunder-bolts
	g_pGamePersistent->Environment.RenderLast	();				// rain/lens-flares/thunder-bolts

	// Postprocess, if necessary
	Target->End						();
	L_Projector->finalize			();

	// HUD
	Device.Statistic.RenderDUMP.End	();
}

void	CRender::ApplyBlur4		(FVF::TL4uv* pv, u32 w, u32 h, float k)
{
	float	_w					= float(w);
	float	_h					= float(h);
	float	kw					= (1.f/_w)*k;
	float	kh					= (1.f/_h)*k;
	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	u32		_c					= 0xffffffff;

	// Fill vertex buffer
	pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->color=_c; pv->uv[0].set(p0.x-kw,p1.y-kh);pv->uv[1].set(p0.x+kw,p1.y+kh);pv->uv[2].set(p0.x+kw,p1.y-kh);pv->uv[3].set(p0.x-kw,p1.y+kh);pv++;
	pv->p.set(EPS,			EPS,			EPS,1.f); pv->color=_c; pv->uv[0].set(p0.x-kw,p0.y-kh);pv->uv[1].set(p0.x+kw,p0.y+kh);pv->uv[2].set(p0.x+kw,p0.y-kh);pv->uv[3].set(p0.x-kw,p0.y+kh);pv++;
	pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->color=_c; pv->uv[0].set(p1.x-kw,p1.y-kh);pv->uv[1].set(p1.x+kw,p1.y+kh);pv->uv[2].set(p1.x+kw,p1.y-kh);pv->uv[3].set(p1.x-kw,p1.y+kh);pv++;
	pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->color=_c; pv->uv[0].set(p1.x-kw,p0.y-kh);pv->uv[1].set(p1.x+kw,p0.y+kh);pv->uv[2].set(p1.x+kw,p0.y-kh);pv->uv[3].set(p1.x-kw,p0.y+kh);pv++;
}

#pragma comment(lib,"d3dx9.lib")

HRESULT	CRender::shader_compile			(
		LPCSTR							name,
		LPCSTR                          pSrcData,
		UINT                            SrcDataLen,
		void*							_pDefines,
		void*							_pInclude,
		LPCSTR                          pFunctionName,
		LPCSTR                          pTarget,
		DWORD                           Flags,
		void*							_ppShader,
		void*							_ppErrorMsgs,
		void*							_ppConstantTable)
{
	D3DXMACRO						defines			[128];
	int								def_it			= 0;
	CONST D3DXMACRO*                pDefines		= (CONST D3DXMACRO*)	_pDefines;
	if (pDefines)	{
		// transfer existing defines
		for (;;def_it++)	{
			if (0==pDefines[def_it].Name)	break;
			defines[def_it]			= pDefines[def_it];
		}
	}
	// options
	if (m_skinning<0)		{
		defines[def_it].Name		=	"SKIN_NONE";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (0==m_skinning)		{
		defines[def_it].Name		=	"SKIN_0";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (1==m_skinning)		{
		defines[def_it].Name		=	"SKIN_1";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (2==m_skinning)		{
		defines[def_it].Name		=	"SKIN_2";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	// finish
	defines[def_it].Name			=	0;
	defines[def_it].Definition		=	0;
	def_it							++;

	LPD3DXINCLUDE                   pInclude		= (LPD3DXINCLUDE)		_pInclude;
	LPD3DXBUFFER*                   ppShader		= (LPD3DXBUFFER*)		_ppShader;
	LPD3DXBUFFER*                   ppErrorMsgs		= (LPD3DXBUFFER*)		_ppErrorMsgs;
	LPD3DXCONSTANTTABLE*            ppConstantTable	= (LPD3DXCONSTANTTABLE*)_ppConstantTable;
	HRESULT		_result	= D3DXCompileShader(pSrcData,SrcDataLen,defines,pInclude,pFunctionName,pTarget,Flags,ppShader,ppErrorMsgs,ppConstantTable);
	if (SUCCEEDED(_result) && o.disasm)
	{
		ID3DXBuffer*		code	= *((LPD3DXBUFFER*)_ppShader);
		ID3DXBuffer*		disasm	= 0;
		D3DXDisassembleShader		(LPDWORD(code->GetBufferPointer()), FALSE, 0, &disasm );
		string_path			dname;
		strconcat			(dname,"disasm\\",name,('v'==pTarget[0])?".vs":".ps" );
		IWriter*			W		= FS.w_open("$logs$",dname);
		W->w				(disasm->GetBufferPointer(),disasm->GetBufferSize());
		FS.w_close			(W);
		_RELEASE			(disasm);
	}
	return		_result;
}
