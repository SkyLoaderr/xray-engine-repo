#include "stdafx.h"
#pragma hdrstop

#include "render.h"
#include "ResourceManager.h"
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

void					CRender::create					()
{
	::Device.Resources->SetHLSL_path("R1\\");
	Models						= xr_new<CModelPool>			();

	PSLibrary.OnCreate			();
	PSLibrary.OnDeviceCreate	();
}
void					CRender::destroy				()
{
	xr_delete					(Models);

	PSLibrary.OnDeviceDestroy	();
	PSLibrary.OnDestroy			();
}

ref_shader	CRender::getShader	(int id){ return 0; }//VERIFY(id<int(Shaders.size()));	return Shaders[id];	}

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
	return Models->CreatePE	(source);
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

void			CRender::add_Visual   		(IRender_Visual* visual)			{ Models->RenderSingle	(visual,current_matrix,1.f);}
IRender_Visual*	CRender::model_Create		(LPCSTR name)						{ return Models->Create(name);			}
IRender_Visual*	CRender::model_Create		(LPCSTR name, IReader* data)		{ return Models->Create(name,data);		}
void 			CRender::model_Delete		(IRender_Visual* &V, BOOL bDiscard)	{ Models->Delete(V,bDiscard);			}
IRender_Visual*	CRender::model_Duplicate	(IRender_Visual* V)					{ return Models->Instance_Duplicate(V);	}
void 			CRender::model_Render		(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD){Models->Render(m_pVisual, mTransform, priority, strictB2F, m_fLOD);}
void 			CRender::model_RenderSingle	(IRender_Visual* m_pVisual, const Fmatrix& mTransform, float m_fLOD){Models->RenderSingle(m_pVisual, mTransform, m_fLOD);}

