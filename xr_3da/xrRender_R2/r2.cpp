#include "stdafx.h"
#include "r2.h"
#include "..\fbasicvisual.h"
#include "..\xr_object.h"
#include "..\CustomHUD.h"
 
CRender										RImplementation;

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

//////////////////////////////////////////////////////////////////////////
// Just two static storage
LPCSTR					r2p(LPCSTR name)
{
	static string128	path;
	if (RImplementation.b_nv3x)	strconcat(path,"r2_nv3x\\",name);
	else						strconcat(path,"r2_r3xx\\",name);
	return				path;
}
LPCSTR					r2v(LPCSTR name)
{
	static string128	path;
	if (RImplementation.b_nv3x)	strconcat(path,"r2_nv3x\\",name);
	else						strconcat(path,"r2_r3xx\\",name);
	return				path;
}

//
void					CRender::create					()
{
	if (b_nv3x)			::Device.Resources->SetHLSL_path("r2_nv3x\\");
	else				::Device.Resources->SetHLSL_path("r2_r3xx\\");
}
void					CRender::destroy				()
{
}

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(IRenderable* parent)				{ return 0;								}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)		{ xr_delete(p);							}
IRender_Visual*			CRender::model_Create			(LPCSTR name)						{ return Models.Create(name);			}
IRender_Visual*			CRender::model_Create			(LPCSTR name, IReader* data)		{ return Models.Create(name,data);		}
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

void					CRender::flush					()					{ r_dsgraph_render_graph	(0);						}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);							}

void					CRender::add_Visual				(IRender_Visual*		V )	{ add_leafs_Dynamic(V);								}
void					CRender::add_Geometry			(IRender_Visual*		V )	{ add_Static(V,View->getMask());					}
void					CRender::add_Wallmark			(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
	//Msg		("! NotImplemented: CRender::add_Wallmark");
}
void					CRender::set_Object			(IRenderable*	O )	
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
	b_nv3x	= (strstr(Core.Params,"-nv3x"))?TRUE:FALSE;
}

CRender::~CRender()
{
}

// Device events
void CRender::OnDeviceCreate	()
{
	REQ_CREATE					();
	Target.OnDeviceCreate		();
	LR.Create					();

	PSLibrary.OnCreate			();
	PSLibrary.OnDeviceCreate	();
	level_Load					();

	rmNormal					();
	marker						= 0;
}

void CRender::OnDeviceDestroy	()
{
	level_Unload				();
	PSLibrary.OnDeviceDestroy	();
	PSLibrary.OnDestroy			();

	LR.Destroy					();
	Target.OnDeviceDestroy		();
}
