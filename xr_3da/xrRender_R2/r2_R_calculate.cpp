#include "stdafx.h"
#include "..\customhud.h"

float				g_fGLOD, g_fFarSq, g_fPOWER;
float				g_fSCREEN;
float				g_fLOD,g_fLOD_scale=1.f;

extern float		r_ssaDISCARD;
extern float		r_ssaDONTSORT;
extern float		r_ssaLOD_A;
extern float		r_ssaLOD_B;
extern float		r_ssaHZBvsTEX;

void CRender::Calculate		()
{
	Device.Statistic.RenderCALC.Begin	();

	// Transfer to global space to avoid deep pointer access
	IRender_Target* T				=	getTarget	();
	g_fFarSq						=	75.f;
	g_fFarSq						*=	g_fFarSq;
	g_fSCREEN						=	float(T->get_width()*T->get_height());
	g_fLOD							=	g_fLOD_scale;
	r_ssaDISCARD					=	_sqr(ps_r2_ssaDISCARD)/g_fSCREEN;
	r_ssaDONTSORT					=	_sqr(ps_r2_ssaDONTSORT)/g_fSCREEN;
	r_ssaLOD_A						=	_sqr(ps_r2_ssaLOD_A)/g_fSCREEN;
	r_ssaLOD_B						=	_sqr(ps_r2_ssaLOD_B)/g_fSCREEN;
	r_ssaHZBvsTEX					=	_sqr(ps_r2_ssaHZBvsTEX)/g_fSCREEN;
	
	// Frustum & HOM rendering
	ViewBase.CreateFromMatrix		(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= 0;
	HOM.Render						(ViewBase);
	rmNormal						();
	
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

	// Calculate sector(s) and their objects
	marker									++;
	set_Object								(0);
	if (0!=pLastSector) pLastSector->Render	(ViewBase);
	pCreator->pHUD->Render_Calculate		();
	
	// End calc
	Device.Statistic.RenderCALC.End			();
}
