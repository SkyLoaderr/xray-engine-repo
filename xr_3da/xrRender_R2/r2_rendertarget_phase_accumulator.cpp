#include "stdafx.h"

void	CRenderTarget::phase_accumulator()
{
	// Targets
	dwWidth								= Device.dwWidth;
	dwHeight							= Device.dwHeight;
	RCache.set_RT						(rt_Accumulator->pRT,	0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(HW.pBaseZB);
	RImplementation.rmNormal			();

	// Clear	- only once per frame
	if (dwAccumulatorClearMark!=Device.dwFrame)
	{
		dwAccumulatorClearMark				= Device.dwFrame;
		CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L));
	}

	// Stencil	- draw only where stencil >= 0x1
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x01				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0x00				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));

	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	
}
