#include "stdafx.h"

void	CRenderTarget::phase_smap_direct		(light* L)
{
	// Targets
	if (RImplementation.o.HW_smap)		u_setrt	(rt_smap_surf, NULL, NULL, rt_smap_depth->pRT);
	else								u_setrt	(rt_smap_surf, NULL, NULL, rt_smap_ZB);

	// Clear
	CHK_DX								(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,	0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only back-faces
	RCache.set_CullMode					( CULL_CCW );
	if (RImplementation.o.HW_smap)		RCache.set_ColorWriteEnable	(FALSE);
}

void	CRenderTarget::phase_smap_direct_tsh	(light* L)
{
	VERIFY								(RImplementation.o.Tshadows);
	RCache.set_ColorWriteEnable			();
	CHK_DX								(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	0xffffffff,	1.0f, 0L));
}
