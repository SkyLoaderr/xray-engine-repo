#include "stdafx.h"

void	CRenderTarget::phase_scene		()
{
	// Targets
	if (RImplementation.b_nv3x)			u_setrt		(rt_Deffer,NULL,NULL,HW.pBaseZB);
	else								u_setrt		(rt_Position,rt_Normal,rt_Color,HW.pBaseZB);

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00, 1.0f, 0L));

	// Stencil - write 0x1 at pixel pos
	RCache.set_Stencil					( TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);

	// Misc		- draw only front-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_TWOSIDEDSTENCILMODE,FALSE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_CCW			));
}
