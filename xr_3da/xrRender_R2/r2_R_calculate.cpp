#include "stdafx.h"
#include "..\customhud.h"

float				g_fSCREEN		;

extern float		r_dtex_range	;
extern float		r_ssaDISCARD	;
extern float		r_ssaDONTSORT	;
extern float		r_ssaLOD_A		;
extern float		r_ssaLOD_B		;
extern float		r_ssaHZBvsTEX	;
extern float		r_ssaGLOD_start,	r_ssaGLOD_end;

void CRender::Calculate		()
{
	// Transfer to global space to avoid deep pointer access
	IRender_Target* T				=	getTarget	();
	float	fov_factor				=	_sqr		(90.f / Device.fFOV);
	g_fSCREEN						=	float(T->get_width()*T->get_height())*fov_factor*(EPS_S+ps_r__LOD);
	r_ssaDISCARD					=	_sqr(ps_r2_ssaDISCARD/2)	/g_fSCREEN;
	r_ssaDONTSORT					=	_sqr(ps_r2_ssaDONTSORT/2)	/g_fSCREEN;
	r_ssaLOD_A						=	_sqr(ps_r2_ssaLOD_A/2)		/g_fSCREEN;
	r_ssaLOD_B						=	_sqr(ps_r2_ssaLOD_B/2)		/g_fSCREEN;
	r_ssaGLOD_start					=	_sqr(ps_r__GLOD_ssa_start/2)/g_fSCREEN;
	r_ssaGLOD_end					=	_sqr(ps_r__GLOD_ssa_end/2)	/g_fSCREEN;
	r_ssaHZBvsTEX					=	_sqr(ps_r2_ssaHZBvsTEX/2)	/g_fSCREEN;
	r_dtex_range					=	ps_r2_df_parallax_range * g_fSCREEN / (1024.f * 768.f);
	
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
		float	eps			= VIEWPORT_NEAR+EPS_L;
		Fvector box_radius; box_radius.set(eps,eps,eps);
		Sectors_xrc.box_options	(0);
		Sectors_xrc.box_query	(rmPortals,Device.vCameraPosition,box_radius);
		for (int K=0; K<Sectors_xrc.r_count(); K++)	{
			CPortal*	pPortal		= (CPortal*) Portals[rmPortals->get_tris()[Sectors_xrc.r_begin()[K].id].dummy];
			pPortal->bDualRender	= TRUE;
		}
	}

	//
	Lights.Update();
}
