#include "stdafx.h"
#pragma hdrstop

#include "render.h"
#include "ResourceManager.h"
//---------------------------------------------------------------------------
float ssaDISCARD		= 4.f;
float ssaDONTSORT		= 32.f;

ECORE_API float r_ssaDISCARD;
ECORE_API float	g_fSCREEN;

CRender 	RImplementation;
CRender* 	Render = &RImplementation;
//---------------------------------------------------------------------------

CRender::CRender	()
{
	m_skinning					= 0;
}

CRender::~CRender	()
{
	xr_delete		(Target);
}

void					CRender::Initialize				()
{
	PSLibrary.OnCreate			();
}
void					CRender::ShutDown				()
{
	PSLibrary.OnDestroy			();
}

void					CRender::OnDeviceCreate			()
{
	Models						= xr_new<CModelPool>	();
    Models->Logging				(FALSE);
}
void					CRender::OnDeviceDestroy		()
{
	xr_delete					(Models);
}

ref_shader	CRender::getShader	(int id){ return 0; }//VERIFY(id<int(Shaders.size()));	return Shaders[id];	}

BOOL CRender::occ_visible(Fbox&	B)
{
	u32 mask		= 0xff;
	return ViewBase.testAABB(B.data(),mask);
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
		PS::CPGDef*	SG	= PSLibrary.FindPGD	(name);
		return			SG?Models->CreatePG	(SG):0;
	}
}

void	CRender::rmNear		()
{
	CRenderTarget* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmFar		()
{
	CRenderTarget* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmNormal	()
{
	CRenderTarget* T	=	getTarget	();
	D3DVIEWPORT9 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}

void 	CRender::set_Transform	(Fmatrix* M)
{
	current_matrix.set(*M);
}

void			CRender::add_Visual   		(IRender_Visual* visual)			{ Models->RenderSingle	(visual,current_matrix,1.f);}
IRender_Visual*	CRender::model_Create		(LPCSTR name, IReader* data)		{ return Models->Create(name,data);		}
IRender_Visual*	CRender::model_CreateChild	(LPCSTR name, IReader* data)		{ return Models->CreateChild(name,data);}
void 			CRender::model_Delete		(IRender_Visual* &V, BOOL bDiscard)	{ Models->Delete(V,bDiscard);			}
IRender_Visual*	CRender::model_Duplicate	(IRender_Visual* V)					{ return Models->Instance_Duplicate(V);	}
void 			CRender::model_Render		(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD){Models->Render(m_pVisual, mTransform, priority, strictB2F, m_fLOD);}
void 			CRender::model_RenderSingle	(IRender_Visual* m_pVisual, const Fmatrix& mTransform, float m_fLOD){Models->RenderSingle(m_pVisual, mTransform, m_fLOD);}

//#pragma comment(lib,"d3dx_r1")
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
        CONST D3DXMACRO*                pDefines		= (CONST D3DXMACRO*)	_pDefines;
        LPD3DXINCLUDE                   pInclude		= (LPD3DXINCLUDE)		_pInclude;
        LPD3DXBUFFER*                   ppShader		= (LPD3DXBUFFER*)		_ppShader;
        LPD3DXBUFFER*                   ppErrorMsgs		= (LPD3DXBUFFER*)		_ppErrorMsgs;
        LPD3DXCONSTANTTABLE*            ppConstantTable	= (LPD3DXCONSTANTTABLE*)_ppConstantTable;
		return D3DXCompileShader		(pSrcData,SrcDataLen,pDefines,pInclude,pFunctionName,pTarget,Flags,ppShader,ppErrorMsgs,ppConstantTable);
}
HRESULT	CRender::shader_compile			(
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
	return D3DXCompileShader		(pSrcData,SrcDataLen,defines,pInclude,pFunctionName,pTarget,Flags,ppShader,ppErrorMsgs,ppConstantTable);
}

void					CRender::reset_begin			()
{
	xr_delete			(Target);
}
void					CRender::reset_end				()
{
	Target			=	xr_new<CRenderTarget>			();
}

