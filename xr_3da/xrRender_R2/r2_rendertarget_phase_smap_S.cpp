#include "stdafx.h"

void	CRenderTarget::phase_smap_spot	()
{
	// Targets
	dwWidth								= DSM_size;
	dwHeight							= DSM_size;
	u_setrt								(rt_smap_d, NULL, NULL, rt_smap_d_ZB);
	RImplementation.rmNormal			();

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, /*D3DCLEAR_TARGET|*/ D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front-faces //back-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			)); 	
}
