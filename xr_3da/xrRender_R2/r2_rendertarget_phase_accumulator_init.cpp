#include "stdafx.h"

void CRenderTarget::phase_accumulator_init()
{
	dwLightMarkerID								= 5;		// start from 5, increment in 2 units
	u32		clr4clear							= color_rgba(0,0,0,0);	// 0x00
	CHK_DX	(HW.pDevice->Clear					( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));

	// Render emissive geometry
	{
		// Stencil - write 0x0 at pixel pos
		RCache.set_Stencil							(TRUE,D3DCMP_ALWAYS,0x00,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.set_CullMode							(CULL_CCW);
		RCache.set_ColorWriteEnable					();
		RImplementation.r_dsgraph_render_emissive	();
	}
}
