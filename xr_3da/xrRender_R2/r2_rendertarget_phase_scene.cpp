#include "stdafx.h"

void	CRenderTarget::phase_scene		()
{
	// Targets
	u_setrt		(rt_Position,	rt_Normal,		rt_Color,HW.pBaseZB);

	// Clear
	if (dwZBufferClearMark!=Device.dwFrame)
	{
		dwZBufferClearMark					= Device.dwFrame;
		CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0L));
	}

	// Stencil - write 0x1 at pixel pos
	RCache.set_Stencil					( TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);

	// Misc		- draw only front-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_TWOSIDEDSTENCILMODE,FALSE				));
	RCache.set_CullMode					( CULL_CCW );
	RCache.set_ColorWriteEnable			( );
}
