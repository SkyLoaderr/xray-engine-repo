#include "stdafx.h"

void	CRenderTarget::phase_scene		()
{
	// Targets
	dwWidth								= Device.dwWidth;
	dwHeight							= Device.dwHeight;
	RCache.set_RT						(rt_Position->pRT,		0);
	RCache.set_RT						(rt_Normal->pRT,		1);
	RCache.set_RT						(rt_Color->pRT,			2);
	RCache.set_ZB						(HW.pBaseZB);
	RImplementation.rmNormal			();

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00, 1.0f, 0L));

	// Stencil - write 0x1 at pixel pos
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		TRUE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_TWOSIDEDSTENCILMODE,FALSE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS		));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			0x1					));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	0xff				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		D3DSTENCILOP_REPLACE));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));

	// Misc
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			));	// draw only front-faces
}
