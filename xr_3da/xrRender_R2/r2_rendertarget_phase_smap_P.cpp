#include "stdafx.h"

void	CRenderTarget::phase_smap_point(u32 pls_phase)
{
	// Targets
	dwWidth								= PSM_size;
	dwHeight							= PSM_size;
	u_setrt								(rt_smap_p->pRT[pls_phase], NULL, NULL, rt_smap_p_ZB);
	RImplementation.rmNormal			();

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			)); 	
}
