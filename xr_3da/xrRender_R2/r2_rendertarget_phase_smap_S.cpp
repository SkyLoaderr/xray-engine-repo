#include "stdafx.h"

void	CRenderTarget::phase_smap_spot	()
{
	// Targets
	if (RImplementation.b_nv3x)			u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_depth->pRT);
	else								u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_ZB);

	// Clear
	if (RImplementation.b_nv3x)			{ CHK_DX(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET/*$$$*/|D3DCLEAR_ZBUFFER,	0x11,		1.0f, 0L)); }
	else								{ CHK_DX(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,		0xFFFFFFFF, 1.0f, 0L)); }

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front-faces //back-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			)); 	
}
