#include "stdafx.h"

void	CRenderTarget::phase_smap_spot	()
{
	// Targets
	dwWidth								= SSM_size;
	dwHeight							= SSM_size;
	u_setrt								(rt_smap_s, NULL, NULL, rt_smap_s_ZB);
	RImplementation.rmNormal			();

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, /*D3DCLEAR_TARGET|*/ D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front-faces //back-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			)); 	
}
