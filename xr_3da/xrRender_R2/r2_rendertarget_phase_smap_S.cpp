#include "stdafx.h"

void	CRenderTarget::phase_smap_spot_clear()
{
	/*
	if (RImplementation.b_HW_smap)		u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_depth->pRT);
	else								u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_ZB);
	CHK_DX								(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,	0xffffffff,	1.0f, 0L));
	*/
}

void	CRenderTarget::phase_smap_spot		(light* L)
{
	// Targets + viewport
	if (RImplementation.b_HW_smap)		u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_depth->pRT);
	else								u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_ZB);
	D3DVIEWPORT9 VP					=	{L->X.S.posX,L->X.S.posY,L->X.S.size,L->X.S.size,0,1 };
	CHK_DX								(HW.pDevice->SetViewport(&VP));

	// Misc		- draw only front-faces //back-faces
	RCache.set_CullMode					( CULL_CCW );
	RCache.set_Stencil					( FALSE );
	// no transparency
	#pragma todo("can optimize for multi-lights covering more than say 50%...")
	CHK_DX							(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,	0xffffffff,	1.0f, 0L));
	if (RImplementation.b_HW_smap)		RCache.set_ColorWriteEnable	(FALSE);
}

void	CRenderTarget::phase_smap_spot_tsh	(light* L)
{
	VERIFY							(RImplementation.b_Tshadows);
	RCache.set_ColorWriteEnable		();

	// fill color-mask
}
