#include "stdafx.h"

void	CRenderTarget::phase_distortion	()
{
	u_setrt						(rt_Generic_1,0,0,HW.pBaseZB);
	RCache.set_CullMode			(CULL_CCW);
	RCache.set_Stencil			(FALSE);
	RCache.set_ColorWriteEnable	();
	CHK_DX(HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, color_rgba(127,127,127,127), 1.0f, 0L));
	RImplementation.r_dsgraph_render_distort	();
}
