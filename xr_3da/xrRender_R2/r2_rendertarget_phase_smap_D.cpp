#include "stdafx.h"

void	CRenderTarget::phase_smap_direct		(light* L, u32 sub_phase)
{
	// Targets
	if (RImplementation.o.HW_smap)		u_setrt	(rt_smap_surf, NULL, NULL, rt_smap_depth->pRT);
	else								u_setrt	(rt_smap_surf, NULL, NULL, rt_smap_ZB);

	// Clear
	CHK_DX								(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,	0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front/back-faces
	// RCache.set_CullMode					( CULL_CCW	);	// near
	if (SE_SUN_NEAR==sub_phase)			RCache.set_CullMode			( CULL_CCW	);	// near
	else								RCache.set_CullMode			( CULL_CW	);	// far
	if (RImplementation.o.HW_smap)		RCache.set_ColorWriteEnable	(FALSE);
}

void	CRenderTarget::phase_smap_direct_tsh	(light* L, u32 sub_phase)
{
	VERIFY								(RImplementation.o.Tshadows);
	u32		_clr						= 0xffffffff;	//color_rgba(127,127,12,12);
	RCache.set_ColorWriteEnable			();
	CHK_DX								(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	_clr,	1.0f, 0L));
}
