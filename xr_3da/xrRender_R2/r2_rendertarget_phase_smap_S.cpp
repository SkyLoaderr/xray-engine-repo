#include "stdafx.h"

void	CRenderTarget::phase_smap_spot	()
{
	// Targets
	dwWidth								= SSM_size;
	dwHeight							= SSM_size;
	RCache.set_RT						(rt_smap_s->pRT,		0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(rt_smap_s_ZB);
	RImplementation.rmNormal			();

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, /*D3DCLEAR_TARGET|*/ D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		FALSE				));

	// Misc		- draw only front-faces //back-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			)); 	
}
