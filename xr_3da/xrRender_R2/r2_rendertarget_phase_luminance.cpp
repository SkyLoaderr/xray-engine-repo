#include "stdafx.h"

void	CRenderTarget::phase_luminance()
{
	// Targets
	u_setrt									(rt_LUM_16x16,NULL,NULL,NULL);
	RCache.set_Stencil						(FALSE);
	RCache.set_CullMode						(CULL_NONE);
	RCache.set_ColorWriteEnable				();
	CHK_DX		(HW.pDevice->SetRenderState	(D3DRS_ZENABLE,FALSE));
	CHK_DX		(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L));

	// Perform LUM-SAT

	CHK_DX		(HW.pDevice->SetRenderState(D3DRS_ZENABLE,TRUE));
}
