#include "stdafx.h"
#pragma hdrstop

#include "render.h"
//---------------------------------------------------------------------------
float ssaDISCARD		= 4.f;
float ssaDONTSORT		= 32.f;

float 	r_ssaDISCARD;
float	g_fSCREEN;

CRender 	Render_implementation;
CRender* 	Render = &Render_implementation;
//---------------------------------------------------------------------------

CRender::CRender	()
{
}

CRender::~CRender	()
{
}

BOOL CRender::occ_visible(Fbox&	B)
{
	DWORD mask		= 0xff;
	return ViewBase.testAABB(B.min,B.max,mask);
}

BOOL CRender::occ_visible(sPoly& P)
{
	return ViewBase.testPolyInside(P);
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
