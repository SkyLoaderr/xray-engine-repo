#include "stdafx.h"

void CRenderTarget::draw_volume		(light* L)
{
	switch(L->flags.type) {
	case IRender_Light::REFLECTED	:
	case IRender_Light::POINT		:
		RCache.set_Geometry		(g_accum_point);
		RCache.Render			(D3DPT_TRIANGLELIST,0,0,DU_SPHERE_NUMVERTEX,0,DU_SPHERE_NUMFACES);
		break;
	case IRender_Light::SPOT	:
		RCache.set_Geometry		(g_accum_spot);
		RCache.Render			(D3DPT_TRIANGLELIST,0,0,DU_CONE_NUMVERTEX,0,DU_CONE_NUMFACES);
		break;
	default:
		break;
	}
}
