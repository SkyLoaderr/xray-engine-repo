#include "stdafx.h"
#include "r2.h"
#include "..\fbasicvisual.h"
#include "..\xr_object.h"
#include "..\CustomHUD.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
 
CRender										RImplementation;

//////////////////////////////////////////////////////////////////////////
class CGlow				: public IRender_Glow
{
public:
	bool				bActive;
public:
	virtual void					set_active			(bool)						{ bActive=TRUE;		}
	virtual bool					get_active			()							{ return bActive;	}
	virtual void					set_position		(const Fvector& P)			{ }
	virtual void					set_direction		(const Fvector& D)			{ }
	virtual void					set_radius			(float R)					{ }
	virtual void					set_texture			(LPCSTR name)				{ }
	virtual void					set_color			(const Fcolor& C)			{ }
	virtual void					set_color			(float r, float g, float b)	{ }
};

//////////////////////////////////////////////////////////////////////////
ShaderElement*			CRender::rimp_select_sh_dynamic	(IRender_Visual	*pVisual, float cdist_sq)
{
	return pVisual->hShader->E[RImplementation.phase]._get();
}
//////////////////////////////////////////////////////////////////////////
ShaderElement*			CRender::rimp_select_sh_static	(IRender_Visual	*pVisual, float cdist_sq)
{
	return pVisual->hShader->E[RImplementation.phase]._get();
}
static class cl_encodeZ01		: public R_constant_setup		{	virtual void setup	(R_constant* C)
{
	float		f	= g_pGamePersistent->Environment.CurrentEnv.far_plane;
	Fvector3	enc = {1,256,65536};
	enc.div			(f);
	RCache.set_c	(C,enc.x,enc.y,enc.z,0.f);
}}	binder_encodeZ01;
static class cl_decodeZ01		: public R_constant_setup		{	virtual void setup	(R_constant* C)
{
	float		f	= g_pGamePersistent->Environment.CurrentEnv.far_plane;
	Fvector3	enc = {f/1.f,f/256.f,f/65536.f};
	enc.mul			(0.995f);
	RCache.set_c	(C,enc.x,enc.y,enc.z,0.f);
}}	binder_decodeZ01;
static class cl_parallax		: public R_constant_setup		{	virtual void setup	(R_constant* C)
{
	float			h			= ps_r2_ls_flags.test(R2FLAG_PARALLAX) ? ps_r2_df_parallaxh:0;
	RCache.set_c	(C,h,-h/2,0.f,0.f);
}}	binder_parallax;
static class cl_lhdrscale		: public R_constant_setup		{	virtual void setup	(R_constant* C)
{
	float dr					= ps_r2_ls_dynamic_range;
	RCache.set_c				(C,	dr,dr,dr,dr);
}}	binder_lhdrscale;
//////////////////////////////////////////////////////////////////////////
// Just two static storage
void					CRender::create					()
{
	::Device.Resources->RegisterConstantSetup	("v_encodeZ01",	&binder_encodeZ01);
	::Device.Resources->RegisterConstantSetup	("v_decodeZ01",	&binder_decodeZ01);
	::Device.Resources->RegisterConstantSetup	("parallax",	&binder_parallax);
	::Device.Resources->RegisterConstantSetup	("L_hdr",		&binder_lhdrscale);

	c_lmaterial					= "L_material";
	c_sbase						= "s_base";

	Target.OnDeviceCreate		();
	LR.Create					();

	PSLibrary.OnCreate			();
	PSLibrary.OnDeviceCreate	();
	HWOCC.occq_create			(occq_size);

	rmNormal					();
	marker						= 0;
} 
void					CRender::destroy				()
{
	HWOCC.occq_destroy			();
	PSLibrary.OnDeviceDestroy	();
	PSLibrary.OnDestroy			();

	LR.Destroy					();
	Target.OnDeviceDestroy		();
}

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(IRenderable* parent)				{ return 0;								}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)		{ xr_delete(p);							}
IRender_Visual*			CRender::model_Create			(LPCSTR name, IReader* data)		{ return Models.Create(name,data);		}
IRender_Visual*			CRender::model_CreateChild		(LPCSTR name, IReader* data)		{ return Models.CreateChild(name,data);	}
IRender_Visual*			CRender::model_Duplicate		(IRender_Visual* V)					{ return Models.Instance_Duplicate(V);	}
void					CRender::model_Delete			(IRender_Visual* &V, BOOL bDiscard)	{ Models.Delete(V, bDiscard);			}
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
	PS::CPEDef*	SE			= PSLibrary.FindPED	(name);		VERIFY(SE);
	return					Models.CreatePE	(SE);
}
IRender_Visual*			CRender::model_CreateParticles	(LPCSTR name)	
{ 
	PS::CPEDef*	SE			= PSLibrary.FindPED	(name);
	if (SE) return			Models.CreatePE	(SE);
	else{
		PS::CPGDef*	SG		= PSLibrary.FindPGD	(name);		R_ASSERT(SG);
		return				Models.CreatePG	(SG);
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
IRender_Target*			CRender::getTarget				()					{ return &Target;										}

IRender_Light*			CRender::light_create			()					{ return Lights.Create();								}
void					CRender::light_destroy			(IRender_Light* &L)	{ if (L) { Lights.Destroy((light*)L); L=0; }			}
IRender_Glow*			CRender::glow_create			()					{ return xr_new<CGlow>();								}
void					CRender::glow_destroy			(IRender_Glow* &L)	{ xr_delete(L);											}		

void					CRender::flush					()					{ r_dsgraph_render_graph	(0);						}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);								}

void					CRender::add_Visual				(IRender_Visual*		V )	{ add_leafs_Dynamic(V);								}
void					CRender::add_Geometry			(IRender_Visual*		V )	{ add_Static(V,View->getMask());					}
void					CRender::add_Wallmark			(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
	//Msg		("! NotImplemented: CRender::add_Wallmark");
}
void					CRender::add_Occluder			(Fbox2&	bb_screenspace	)
{
	HOM.occlude			(bb_screenspace);
}
void					CRender::set_Object				(IRenderable*	O )	
{ 
	val_pObject				= O;
}
void					CRender::rmNear				()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void					CRender::rmFar				()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void					CRender::rmNormal			()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender()
{
	b_mrt				= TRUE;
	b_nv3x				= (strstr(Core.Params,"-nv3x"))?TRUE:FALSE;
	b_HW_smap			= (strstr(Core.Params,"-hw_smap"))?TRUE:FALSE;
	b_noshadows			= (strstr(Core.Params,"-noshadows"))?TRUE:FALSE;
	b_fp16				= (strstr(Core.Params,"-fp16"))?TRUE:FALSE;
}

CRender::~CRender()
{
}

#include "..\GameFont.h"
void	CRender::Statistics	(CGameFont* _F)
{
	CGameFont&	F	= *_F;
	F.OutNext	(" **** LT:%2d,LV:%2d **** ",stats.l_total,stats.l_visible		);	stats.l_visible = 0;
	F.OutNext	("    S(%2d)   | (%2d)NS   ",stats.l_shadowed,stats.l_unshadowed);
	F.OutNext	(" omni(%2d)   | (%2d)omni ",stats.l_point_s,stats.l_point		);
	F.OutNext	(" spot(%2d)   | (%2d)spot ",stats.l_spot_s,stats.l_spot		);
	F.OutSkip	();
	F.OutNext	(" **** Occ-Q(%03.1f) **** ",100.f*f32(stats.o_culled)/f32(stats.o_queries?stats.o_queries:1));
	F.OutNext	(" total  : %2d",	stats.o_queries	);	stats.o_queries = 0;
	F.OutNext	(" culled : %2d",	stats.o_culled	);	stats.o_culled	= 0;
}

/////////
#pragma comment(lib,"d3dx_r2")

extern "C"
{
	LPCSTR WINAPI	D3DXGetPixelShaderProfile	(LPDIRECT3DDEVICE9  pDevice);
	LPCSTR WINAPI	D3DXGetVertexShaderProfile	(LPDIRECT3DDEVICE9	pDevice);
};

HRESULT	CRender::CompileShader			(
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
	if (b_fp16)	{
		defines[def_it].Name		=	"FP16_DEFER";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	if (b_HW_smap) {
		defines[def_it].Name		=	"USE_HWSMAP";
		defines[def_it].Definition	=	"1";
		def_it						++;
	}
	// finish
	defines[def_it].Name			=	0;
	defines[def_it].Definition		=	0;
	def_it							++;

	// 
	if ('v'==pTarget[0])			pTarget = D3DXGetVertexShaderProfile	(HW.pDevice);	// vertex
	else							pTarget = D3DXGetPixelShaderProfile		(HW.pDevice);	// pixel

	LPD3DXINCLUDE                   pInclude		= (LPD3DXINCLUDE)		_pInclude;
    LPD3DXBUFFER*                   ppShader		= (LPD3DXBUFFER*)		_ppShader;
    LPD3DXBUFFER*                   ppErrorMsgs		= (LPD3DXBUFFER*)		_ppErrorMsgs;
    LPD3DXCONSTANTTABLE*            ppConstantTable	= (LPD3DXCONSTANTTABLE*)_ppConstantTable;
	return D3DXCompileShader		(pSrcData,SrcDataLen,defines,pInclude,pFunctionName,pTarget,9,ppShader,ppErrorMsgs,ppConstantTable);
}
