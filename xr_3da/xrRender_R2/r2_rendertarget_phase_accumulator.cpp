#include "stdafx.h"

void	CRenderTarget::phase_accumulator()
{
	// Targets
	u_setrt								(rt_Accumulator,NULL,NULL,HW.pBaseZB);

	// Clear and mask	- only once per frame
	// Assuming next usage will be for directional light - apply mask 
	if (dwAccumulatorClearMark==Device.dwFrame)	return;
	dwAccumulatorClearMark				= Device.dwFrame;
	phase_accumulator_init				();

	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
	RCache.set_CullMode					(CULL_NONE);
	RCache.set_ColorWriteEnable			();
}
