#include "stdafx.h"

void	CRenderTarget::phase_accumulator()
{
	// Targets
	u_setrt								(rt_Accumulator,NULL,NULL,HW.pBaseZB);

	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);

	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	

	// Clear and mask	- only once per frame
	// Assuming next usage will be for directional light - apply mask 
	if (dwAccumulatorClearMark==Device.dwFrame)	return;
	dwAccumulatorClearMark				= Device.dwFrame;
	phase_accumulator_init				();

	// Restore targets
	u_setrt								(rt_Accumulator,NULL,NULL,HW.pBaseZB);

	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
}
