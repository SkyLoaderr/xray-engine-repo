#include "stdafx.h"
#pragma hdrstop

#include "render.h"
//---------------------------------------------------------------------------
float ssaDISCARD		= 4.f;
float ssaDONTSORT		= 32.f;

float 	r_ssaDISCARD;
float	g_fSCREEN;

CRender 	RImplementation;
CRender* 	Render = &RImplementation;
//---------------------------------------------------------------------------

CRender::CRender	()
{
}

CRender::~CRender	()
{
}

BOOL CRender::occ_visible(Fbox&	B)
{
	u32 mask		= 0xff;
	return ViewBase.testAABB(B.min,B.max,mask);
}

BOOL CRender::occ_visible(sPoly& P)
{
	return ViewBase.testPolyInside(P);
}

BOOL CRender::occ_visible(vis_data& P)
{
	return occ_visible(P.box);
}

void CRender::Calculate()
{
	// Transfer to global space to avoid deep pointer access
	g_fSCREEN						=	float(Device.dwWidth*Device.dwHeight);
	r_ssaDISCARD					=	(ssaDISCARD*ssaDISCARD)/g_fSCREEN;
//	r_ssaLOD_A						=	(ssaLOD_A*ssaLOD_A)/g_fSCREEN;
//	r_ssaLOD_B						=	(ssaLOD_B*ssaLOD_B)/g_fSCREEN;

	ViewBase.CreateFromMatrix		(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
}

void CRender::Render()
{
}

IRender_DetailModel*	CRender::model_CreateDM(IReader* F)
{
	CDetail*	D		= xr_new<CDetail> ();
	D->Load				(F);
	return D;
}

IRender_Visual*	CRender::model_CreatePE(LPCSTR name)	
{ 
	PS::CPEDef*	source		= PSLibrary.FindPED	(name);
	return Device.Models.CreatePE	(source);
}

IRender_Visual*			CRender::model_CreateParticles	(LPCSTR name)	
{ 
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);
	if (SE) return		Device.Models.CreatePE	(SE);
	else{
		PS::CPGDef*	SG	= PSLibrary.FindPGD	(name);		R_ASSERT(SG);
		return			Device.Models.CreatePG	(SG);
	}
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

void 	CRender::set_Transform	(Fmatrix* M)
{
	current_matrix.set(*M);
}

void	CRender::add_Visual   	(IRender_Visual* visual)
{
    Device.Models.RenderSingle	(visual,current_matrix);
}

